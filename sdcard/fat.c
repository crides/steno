
/* 
 * Copyright (c) 2006-2012 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "byteordering.h"
#include "partition.h"
#include "fat.h"
#include "fat_config.h"
#include "sd-reader_config.h"
#include "sd_raw.h"

#include <string.h>

#if USE_DYNAMIC_MEMORY
    #include <stdlib.h>
#endif

/**
 * \addtogroup fat FAT support
 *
 * This module implements FAT16/FAT32 read and write access.
 * 
 * The following features are supported:
 * - File names up to 31 characters long.
 * - Unlimited depth of subdirectories.
 * - Short 8.3 and long filenames.
 * - Creating and deleting files.
 * - Reading and writing from and to files.
 * - File resizing.
 * - File sizes of up to 4 gigabytes.
 * 
 * @{
 */
/**
 * \file
 * FAT implementation (license: GPLv2 or LGPLv2.1)
 *
 * \author Roland Riegel
 */

/**
 * \addtogroup fat_config FAT configuration
 * Preprocessor defines to configure the FAT implementation.
 */

/**
 * \addtogroup fat_fs FAT access
 * Basic functions for handling a FAT filesystem.
 */

/**
 * \addtogroup fat_file FAT file functions
 * Functions for managing files.
 */

/**
 * \addtogroup fat_dir FAT directory functions
 * Functions for managing directories.
 */

/**
 * @}
 */

#define FAT16_CLUSTER_FREE 0x0000
#define FAT16_CLUSTER_RESERVED_MIN 0xfff0
#define FAT16_CLUSTER_RESERVED_MAX 0xfff6
#define FAT16_CLUSTER_BAD 0xfff7
#define FAT16_CLUSTER_LAST_MIN 0xfff8
#define FAT16_CLUSTER_LAST_MAX 0xffff

#define FAT32_CLUSTER_FREE 0x00000000
#define FAT32_CLUSTER_RESERVED_MIN 0x0ffffff0
#define FAT32_CLUSTER_RESERVED_MAX 0x0ffffff6
#define FAT32_CLUSTER_BAD 0x0ffffff7
#define FAT32_CLUSTER_LAST_MIN 0x0ffffff8
#define FAT32_CLUSTER_LAST_MAX 0x0fffffff

#define FAT_DIRENTRY_DELETED 0xe5
#define FAT_DIRENTRY_LFNLAST (1 << 6)
#define FAT_DIRENTRY_LFNSEQMASK ((1 << 6) - 1)

/* Each entry within the directory table has a size of 32 bytes
 * and either contains a 8.3 DOS-style file name or a part of a
 * long file name, which may consist of several directory table
 * entries at once.
 *
 * multi-byte integer values are stored little-endian!
 *
 * 8.3 file name entry:
 * ====================
 * offset  length  description
 *      0       8  name (space padded)
 *      8       3  extension (space padded)
 *     11       1  attributes (FAT_ATTRIB_*)
 *
 * long file name (lfn) entry ordering for a single file name:
 * ===========================================================
 * LFN entry n
 *     ...
 * LFN entry 2
 * LFN entry 1
 * 8.3 entry (see above)
 * 
 * lfn entry:
 * ==========
 * offset  length  description
 *      0       1  ordinal field
 *      1       2  unicode character 1
 *      3       3  unicode character 2
 *      5       3  unicode character 3
 *      7       3  unicode character 4
 *      9       3  unicode character 5
 *     11       1  attribute (always 0x0f)
 *     12       1  type (reserved, always 0)
 *     13       1  checksum
 *     14       2  unicode character 6
 *     16       2  unicode character 7
 *     18       2  unicode character 8
 *     20       2  unicode character 9
 *     22       2  unicode character 10
 *     24       2  unicode character 11
 *     26       2  cluster (unused, always 0)
 *     28       2  unicode character 12
 *     30       2  unicode character 13
 * 
 * The ordinal field contains a descending number, from n to 1.
 * For the n'th lfn entry the ordinal field is or'ed with 0x40.
 * For deleted lfn entries, the ordinal field is set to 0xe5.
 */

struct fat_header_struct
{
    offset_t size;

    offset_t fat_offset;
    uint32_t fat_size;

    uint16_t sector_size;
    uint16_t cluster_size;

    offset_t cluster_zero_offset;

    offset_t root_dir_offset;
    cluster_t root_dir_cluster;
};

struct fat_fs_struct
{
    struct partition_struct* partition;
    struct fat_header_struct header;
    cluster_t cluster_free;
};

struct fat_file_struct
{
    struct fat_fs_struct* fs;
    struct fat_dir_entry_struct dir_entry;
    offset_t pos;
    cluster_t pos_cluster;
};

struct fat_dir_struct
{
    struct fat_fs_struct* fs;
    struct fat_dir_entry_struct dir_entry;
    cluster_t entry_cluster;
    uint16_t entry_offset;
};

struct fat_read_dir_callback_arg
{
    struct fat_dir_entry_struct* dir_entry;
    uintptr_t bytes_read;
#if FAT_LFN_SUPPORT
    uint8_t checksum;
#endif
    uint8_t finished;
};

struct fat_usage_count_callback_arg
{
    cluster_t cluster_count;
    uintptr_t buffer_size;
};

#if !USE_DYNAMIC_MEMORY
static struct fat_fs_struct fat_fs_handles[FAT_FS_COUNT];
static struct fat_file_struct fat_file_handles[FAT_FILE_COUNT];
static struct fat_dir_struct fat_dir_handles[FAT_DIR_COUNT];
#endif

static uint8_t fat_read_header(struct fat_fs_struct* fs);
static cluster_t fat_get_next_cluster(const struct fat_fs_struct* fs, cluster_t cluster_num);
static offset_t fat_cluster_offset(const struct fat_fs_struct* fs, cluster_t cluster_num);
static uint8_t fat_dir_entry_read_callback(uint8_t* buffer, offset_t offset, void* p);
#if FAT_LFN_SUPPORT
static uint8_t fat_calc_83_checksum(const uint8_t* file_name_83);
#endif

static uint8_t fat_get_fs_free_16_callback(uint8_t* buffer, offset_t offset, void* p);
static uint8_t fat_get_fs_free_32_callback(uint8_t* buffer, offset_t offset, void* p);

/**
 * \ingroup fat_fs
 * Opens a FAT filesystem.
 *
 * \param[in] partition Discriptor of partition on which the filesystem resides.
 * \returns 0 on error, a FAT filesystem descriptor on success.
 * \see fat_close
 */
struct fat_fs_struct* fat_open(struct partition_struct* partition)
{
#if USE_DYNAMIC_MEMORY
    struct fat_fs_struct* fs = malloc(sizeof(*fs));
    if(!fs)
        return 0;
#else
    struct fat_fs_struct* fs = fat_fs_handles;
    uint8_t i;
    for(i = 0; i < FAT_FS_COUNT; ++i)
    {
        if(!fs->partition)
            break;

        ++fs;
    }
    if(i >= FAT_FS_COUNT)
        return 0;
#endif

    memset(fs, 0, sizeof(*fs));

    fs->partition = partition;
    if(!fat_read_header(fs))
    {
#if USE_DYNAMIC_MEMORY
        free(fs);
#else
        fs->partition = 0;
#endif
        return 0;
    }
    
    return fs;
}

/**
 * \ingroup fat_fs
 * Closes a FAT filesystem.
 *
 * When this function returns, the given filesystem descriptor
 * will be invalid.
 *
 * \param[in] fs The filesystem to close.
 * \see fat_open
 */
void fat_close(struct fat_fs_struct* fs)
{
    if(!fs)
        return;

#if USE_DYNAMIC_MEMORY
    free(fs);
#else
    fs->partition = 0;
#endif
}

/**
 * \ingroup fat_fs
 * Reads and parses the header of a FAT filesystem.
 *
 * \param[in,out] fs The filesystem for which to parse the header.
 * \returns 0 on failure, 1 on success.
 */
uint8_t fat_read_header(struct fat_fs_struct* fs)
{
    if(!fs)
        return 0;

    struct partition_struct* partition = fs->partition;
    if(!partition)
        return 0;

    /* read fat parameters */
    uint8_t buffer[37];
    offset_t partition_offset = (offset_t) partition->offset * 512;
    if(!sd_raw_read(partition_offset + 0x0b, buffer, sizeof(buffer)))
        return 0;

    uint16_t bytes_per_sector = read16(&buffer[0x00]);
    uint16_t reserved_sectors = read16(&buffer[0x03]);
    uint8_t sectors_per_cluster = buffer[0x02];
    uint8_t fat_copies = buffer[0x05];
    uint16_t max_root_entries = read16(&buffer[0x06]);
    uint16_t sector_count_16 = read16(&buffer[0x08]);
    uint16_t sectors_per_fat = read16(&buffer[0x0b]);
    uint32_t sector_count = read32(&buffer[0x15]);
    uint32_t sectors_per_fat32 = read32(&buffer[0x19]);
    uint32_t cluster_root_dir = read32(&buffer[0x21]);

    if(sector_count == 0)
    {
        if(sector_count_16 == 0)
            /* illegal volume size */
            return 0;
        else
            sector_count = sector_count_16;
    }
    if(sectors_per_fat != 0)
        sectors_per_fat32 = sectors_per_fat;
    else if(sectors_per_fat32 == 0)
        /* this is neither FAT16 nor FAT32 */
        return 0;

    /* determine the type of FAT we have here */
    uint32_t data_sector_count = sector_count
                                 - reserved_sectors
                                 - sectors_per_fat32 * fat_copies
                                 - ((max_root_entries * 32 + bytes_per_sector - 1) / bytes_per_sector);
    uint32_t data_cluster_count = data_sector_count / sectors_per_cluster;
    if(data_cluster_count < 4085)
        /* this is a FAT12, not supported */
        return 0;
    else if(data_cluster_count < 65525)
        /* this is a FAT16 */
        partition->type = PARTITION_TYPE_FAT16;
    else
        /* this is a FAT32 */
        partition->type = PARTITION_TYPE_FAT32;

    /* fill header information */
    struct fat_header_struct* header = &fs->header;
    memset(header, 0, sizeof(*header));
    
    header->size = (offset_t) sector_count * bytes_per_sector;

    header->fat_offset = /* jump to partition */
                         partition_offset +
                         /* jump to fat */
                         (offset_t) reserved_sectors * bytes_per_sector;
    header->fat_size = (data_cluster_count + 2) * (partition->type == PARTITION_TYPE_FAT16 ? 2 : 4);

    header->sector_size = bytes_per_sector;
    header->cluster_size = (uint16_t) bytes_per_sector * sectors_per_cluster;

    if(partition->type == PARTITION_TYPE_FAT16)
    {
        header->root_dir_offset = /* jump to fats */
                                  header->fat_offset +
                                  /* jump to root directory entries */
                                  (offset_t) fat_copies * sectors_per_fat * bytes_per_sector;

        header->cluster_zero_offset = /* jump to root directory entries */
                                      header->root_dir_offset +
                                      /* skip root directory entries */
                                      (offset_t) max_root_entries * 32;
    }
    else
    {
        header->cluster_zero_offset = /* jump to fats */
                                      header->fat_offset +
                                      /* skip fats */
                                      (offset_t) fat_copies * sectors_per_fat32 * bytes_per_sector;

        header->root_dir_cluster = cluster_root_dir;
    }

    return 1;
}

/**
 * \ingroup fat_fs
 * Retrieves the next following cluster of a given cluster.
 *
 * Using the filesystem file allocation table, this function returns
 * the number of the cluster containing the data directly following
 * the data within the cluster with the given number.
 *
 * \param[in] fs The filesystem for which to determine the next cluster.
 * \param[in] cluster_num The number of the cluster for which to determine its successor.
 * \returns The wanted cluster number, or 0 on error.
 */
cluster_t fat_get_next_cluster(const struct fat_fs_struct* fs, cluster_t cluster_num)
{
    if(!fs || cluster_num < 2)
        return 0;

    if(fs->partition->type == PARTITION_TYPE_FAT32)
    {
        /* read appropriate fat entry */
        uint32_t fat_entry;
        if(!sd_raw_read(fs->header.fat_offset + (offset_t) cluster_num * sizeof(fat_entry), (uint8_t*) &fat_entry, sizeof(fat_entry)))
            return 0;

        /* determine next cluster from fat */
        cluster_num = ltoh32(fat_entry);
        
        if(cluster_num == FAT32_CLUSTER_FREE ||
           cluster_num == FAT32_CLUSTER_BAD ||
           (cluster_num >= FAT32_CLUSTER_RESERVED_MIN && cluster_num <= FAT32_CLUSTER_RESERVED_MAX) ||
           (cluster_num >= FAT32_CLUSTER_LAST_MIN && cluster_num <= FAT32_CLUSTER_LAST_MAX))
            return 0;
    }
    else
    {
        /* read appropriate fat entry */
        uint16_t fat_entry;
        if(!sd_raw_read(fs->header.fat_offset + (offset_t) cluster_num * sizeof(fat_entry), (uint8_t*) &fat_entry, sizeof(fat_entry)))
            return 0;

        /* determine next cluster from fat */
        cluster_num = ltoh16(fat_entry);
        
        if(cluster_num == FAT16_CLUSTER_FREE ||
           cluster_num == FAT16_CLUSTER_BAD ||
           (cluster_num >= FAT16_CLUSTER_RESERVED_MIN && cluster_num <= FAT16_CLUSTER_RESERVED_MAX) ||
           (cluster_num >= FAT16_CLUSTER_LAST_MIN && cluster_num <= FAT16_CLUSTER_LAST_MAX))
            return 0;
    }

    return cluster_num;
}

/**
 * \ingroup fat_fs
 * Calculates the offset of the specified cluster.
 *
 * \param[in] fs The filesystem on which to operate.
 * \param[in] cluster_num The cluster whose offset to calculate.
 * \returns The cluster offset.
 */
offset_t fat_cluster_offset(const struct fat_fs_struct* fs, cluster_t cluster_num)
{
    if(!fs || cluster_num < 2)
        return 0;

    return fs->header.cluster_zero_offset + (offset_t) (cluster_num - 2) * fs->header.cluster_size;
}

/**
 * \ingroup fat_file
 * Retrieves the directory entry of a path.
 *
 * The given path may both describe a file or a directory.
 *
 * \param[in] fs The FAT filesystem on which to search.
 * \param[in] path The path of which to read the directory entry.
 * \param[out] dir_entry The directory entry to fill.
 * \returns 0 on failure, 1 on success.
 * \see fat_read_dir
 */
uint8_t fat_get_dir_entry_of_path(struct fat_fs_struct* fs, const char* path, struct fat_dir_entry_struct* dir_entry)
{
    if(!fs || !path || path[0] == '\0' || !dir_entry)
        return 0;

    if(path[0] == '/')
        ++path;

    /* begin with the root directory */
    memset(dir_entry, 0, sizeof(*dir_entry));
    dir_entry->attributes = FAT_ATTRIB_DIR;

    while(1)
    {
        if(path[0] == '\0')
            return 1;

        struct fat_dir_struct* dd = fat_open_dir(fs, dir_entry);
        if(!dd)
            break;

        /* extract the next hierarchy we will search for */
        const char* sub_path = strchr(path, '/');
        uint8_t length_to_sep;
        if(sub_path)
        {
            length_to_sep = sub_path - path;
            ++sub_path;
        }
        else
        {
            length_to_sep = strlen(path);
            sub_path = path + length_to_sep;
        }
        
        /* read directory entries */
        while(fat_read_dir(dd, dir_entry))
        {
            /* check if we have found the next hierarchy */
            if((strlen(dir_entry->long_name) != length_to_sep ||
                strncmp(path, dir_entry->long_name, length_to_sep) != 0))
                continue;

            fat_close_dir(dd);
            dd = 0;

            if(path[length_to_sep] == '\0')
                /* we iterated through the whole path and have found the file */
                return 1;

            if(dir_entry->attributes & FAT_ATTRIB_DIR)
            {
                /* we found a parent directory of the file we are searching for */
                path = sub_path;
                break;
            }

            /* a parent of the file exists, but not the file itself */
            return 0;
        }

        fat_close_dir(dd);
    }
    
    return 0;
}

/**
 * \ingroup fat_file
 * Opens a file on a FAT filesystem.
 *
 * \param[in] fs The filesystem on which the file to open lies.
 * \param[in] dir_entry The directory entry of the file to open.
 * \returns The file handle, or 0 on failure.
 * \see fat_close_file
 */
struct fat_file_struct* fat_open_file(struct fat_fs_struct* fs, const struct fat_dir_entry_struct* dir_entry)
{
    if(!fs || !dir_entry || (dir_entry->attributes & FAT_ATTRIB_DIR))
        return 0;

#if USE_DYNAMIC_MEMORY
    struct fat_file_struct* fd = malloc(sizeof(*fd));
    if(!fd)
        return 0;
#else
    struct fat_file_struct* fd = fat_file_handles;
    uint8_t i;
    for(i = 0; i < FAT_FILE_COUNT; ++i)
    {
        if(!fd->fs)
            break;

        ++fd;
    }
    if(i >= FAT_FILE_COUNT)
        return 0;
#endif
    
    memcpy(&fd->dir_entry, dir_entry, sizeof(*dir_entry));
    fd->fs = fs;
    fd->pos = 0;
    fd->pos_cluster = dir_entry->cluster;

    return fd;
}

/**
 * \ingroup fat_file
 * Closes a file.
 *
 * \param[in] fd The file handle of the file to close.
 * \see fat_open_file
 */
void fat_close_file(struct fat_file_struct* fd)
{
    if(fd)
    {
#if FAT_DELAY_DIRENTRY_UPDATE
        /* write directory entry */
        fat_write_dir_entry(fd->fs, &fd->dir_entry);
#endif

#if USE_DYNAMIC_MEMORY
        free(fd);
#else
        fd->fs = 0;
#endif
    }
}

/**
 * \ingroup fat_file
 * Reads data from a file.
 * 
 * The data requested is read from the current file location.
 *
 * \param[in] fd The file handle of the file from which to read.
 * \param[out] buffer The buffer into which to write.
 * \param[in] buffer_len The amount of data to read.
 * \returns The number of bytes read, 0 on end of file, or -1 on failure.
 * \see fat_write_file
 */
intptr_t fat_read_file(struct fat_file_struct* fd, uint8_t* buffer, uintptr_t buffer_len)
{
    /* check arguments */
    if(!fd || !buffer || buffer_len < 1)
        return -1;

    /* determine number of bytes to read */
    if(fd->pos + buffer_len > fd->dir_entry.file_size)
        buffer_len = fd->dir_entry.file_size - fd->pos;
    if(buffer_len == 0)
        return 0;
    
    uint16_t cluster_size = fd->fs->header.cluster_size;
    cluster_t cluster_num = fd->pos_cluster;
    uintptr_t buffer_left = buffer_len;
    uint16_t first_cluster_offset = (uint16_t) (fd->pos & (cluster_size - 1));

    /* find cluster in which to start reading */
    if(!cluster_num)
    {
        cluster_num = fd->dir_entry.cluster;
        
        if(!cluster_num)
        {
            if(!fd->pos)
                return 0;
            else
                return -1;
        }

        if(fd->pos)
        {
            uint32_t pos = fd->pos;
            while(pos >= cluster_size)
            {
                pos -= cluster_size;
                cluster_num = fat_get_next_cluster(fd->fs, cluster_num);
                if(!cluster_num)
                    return -1;
            }
        }
    }
    
    /* read data */
    do
    {
        /* calculate data size to copy from cluster */
        offset_t cluster_offset = fat_cluster_offset(fd->fs, cluster_num) + first_cluster_offset;
        uint16_t copy_length = cluster_size - first_cluster_offset;
        if(copy_length > buffer_left)
            copy_length = buffer_left;

        /* read data */
        if(!sd_raw_read(cluster_offset, buffer, copy_length))
            return buffer_len - buffer_left;

        /* calculate new file position */
        buffer += copy_length;
        buffer_left -= copy_length;
        fd->pos += copy_length;

        if(first_cluster_offset + copy_length >= cluster_size)
        {
            /* we are on a cluster boundary, so get the next cluster */
            if((cluster_num = fat_get_next_cluster(fd->fs, cluster_num)))
            {
                first_cluster_offset = 0;
            }
            else
            {
                fd->pos_cluster = 0;
                return buffer_len - buffer_left;
            }
        }

        fd->pos_cluster = cluster_num;

    } while(buffer_left > 0); /* check if we are done */

    return buffer_len;
}

/**
 * \ingroup fat_file
 * Repositions the read/write file offset.
 *
 * Changes the file offset where the next call to fat_read_file()
 * or fat_write_file() starts reading/writing.
 *
 * If the new offset is beyond the end of the file, fat_resize_file()
 * is implicitly called, i.e. the file is expanded.
 *
 * The new offset can be given in different ways determined by
 * the \c whence parameter:
 * - \b FAT_SEEK_SET: \c *offset is relative to the beginning of the file.
 * - \b FAT_SEEK_CUR: \c *offset is relative to the current file position.
 * - \b FAT_SEEK_END: \c *offset is relative to the end of the file.
 *
 * The resulting absolute offset is written to the location the \c offset
 * parameter points to.
 *
 * Calling this function can also be used to retrieve the current file position:
   \code
   int32_t file_pos = 0;
   if(!fat_seek_file(fd, &file_pos, FAT_SEEK_CUR))
   {
       // error
   }
   // file_pos now contains the absolute file position
   \endcode
 * 
 * \param[in] fd The file decriptor of the file on which to seek.
 * \param[in,out] offset A pointer to the new offset, as affected by the \c whence
 *                   parameter. The function writes the new absolute offset
 *                   to this location before it returns.
 * \param[in] whence Affects the way \c offset is interpreted, see above.
 * \returns 0 on failure, 1 on success.
 */
uint8_t fat_seek_file(struct fat_file_struct* fd, int32_t* offset, uint8_t whence)
{
    if(!fd || !offset)
        return 0;

    uint32_t new_pos = fd->pos;
    switch(whence)
    {
        case FAT_SEEK_SET:
            new_pos = *offset;
            break;
        case FAT_SEEK_CUR:
            new_pos += *offset;
            break;
        case FAT_SEEK_END:
            new_pos = fd->dir_entry.file_size + *offset;
            break;
        default:
            return 0;
    }

    if(new_pos > fd->dir_entry.file_size)
        return 0;

    fd->pos = new_pos;
    fd->pos_cluster = 0;

    *offset = (int32_t) new_pos;
    return 1;
}

/**
 * \ingroup fat_dir
 * Opens a directory.
 *
 * \param[in] fs The filesystem on which the directory to open resides.
 * \param[in] dir_entry The directory entry which stands for the directory to open.
 * \returns An opaque directory descriptor on success, 0 on failure.
 * \see fat_close_dir
 */
struct fat_dir_struct* fat_open_dir(struct fat_fs_struct* fs, const struct fat_dir_entry_struct* dir_entry)
{
    if(!fs || !dir_entry || !(dir_entry->attributes & FAT_ATTRIB_DIR))
        return 0;

#if USE_DYNAMIC_MEMORY
    struct fat_dir_struct* dd = malloc(sizeof(*dd));
    if(!dd)
        return 0;
#else
    struct fat_dir_struct* dd = fat_dir_handles;
    uint8_t i;
    for(i = 0; i < FAT_DIR_COUNT; ++i)
    {
        if(!dd->fs)
            break;

        ++dd;
    }
    if(i >= FAT_DIR_COUNT)
        return 0;
#endif
    
    memcpy(&dd->dir_entry, dir_entry, sizeof(*dir_entry));
    dd->fs = fs;
    dd->entry_cluster = dir_entry->cluster;
    dd->entry_offset = 0;

    return dd;
}

/**
 * \ingroup fat_dir
 * Closes a directory descriptor.
 *
 * This function destroys a directory descriptor which was
 * previously obtained by calling fat_open_dir(). When this
 * function returns, the given descriptor will be invalid.
 *
 * \param[in] dd The directory descriptor to close.
 * \see fat_open_dir
 */
void fat_close_dir(struct fat_dir_struct* dd)
{
    if(dd)
#if USE_DYNAMIC_MEMORY
        free(dd);
#else
        dd->fs = 0;
#endif
}

/**
 * \ingroup fat_dir
 * Reads the next directory entry contained within a parent directory.
 *
 * \param[in] dd The descriptor of the parent directory from which to read the entry.
 * \param[out] dir_entry Pointer to a buffer into which to write the directory entry information.
 * \returns 0 on failure, 1 on success.
 * \see fat_reset_dir
 */
uint8_t fat_read_dir(struct fat_dir_struct* dd, struct fat_dir_entry_struct* dir_entry)
{
    if(!dd || !dir_entry)
        return 0;

    /* get current position of directory handle */
    struct fat_fs_struct* fs = dd->fs;
    const struct fat_header_struct* header = &fs->header;
    uint16_t cluster_size = header->cluster_size;
    cluster_t cluster_num = dd->entry_cluster;
    uint16_t cluster_offset = dd->entry_offset;
    struct fat_read_dir_callback_arg arg;

    if(cluster_offset >= cluster_size)
    {
        /* The latest call hit the border of the last cluster in
         * the chain, but it still returned a directory entry.
         * So we now reset the handle and signal the caller the
         * end of the listing.
         */
        fat_reset_dir(dd);
        return 0;
    }

    /* reset callback arguments */
    memset(&arg, 0, sizeof(arg));
    memset(dir_entry, 0, sizeof(*dir_entry));
    arg.dir_entry = dir_entry;

    /* check if we read from the root directory */
    if(cluster_num == 0)
    {
        if(fs->partition->type == PARTITION_TYPE_FAT32)
            cluster_num = header->root_dir_cluster;
        else
            cluster_size = header->cluster_zero_offset - header->root_dir_offset;
    }

    /* read entries */
    uint8_t buffer[32];
    while(!arg.finished)
    {
        /* read directory entries up to the cluster border */
        uint16_t cluster_left = cluster_size - cluster_offset;
        offset_t pos = cluster_offset;
        if(cluster_num == 0)
            pos += header->root_dir_offset;
        else
            pos += fat_cluster_offset(fs, cluster_num);

        arg.bytes_read = 0;
        if(!sd_raw_read_interval(pos,
                                buffer,
                                sizeof(buffer),
                                cluster_left,
                                fat_dir_entry_read_callback,
                                &arg)
          )
            return 0;

        cluster_offset += arg.bytes_read;

        if(cluster_offset >= cluster_size)
        {
            /* we reached the cluster border and switch to the next cluster */

            /* get number of next cluster */
            if((cluster_num = fat_get_next_cluster(fs, cluster_num)) != 0)
            {
                cluster_offset = 0;
                continue;
            }

            /* we are at the end of the cluster chain */
            if(!arg.finished)
            {
                /* directory entry not found, reset directory handle */
                fat_reset_dir(dd);
                return 0;
            }
            else
            {
                /* The current execution of the function has been successful,
                 * so we can not signal an end of the directory listing to
                 * the caller, but must wait for the next call. So we keep an
                 * invalid cluster offset to mark this directory handle's
                 * traversal as finished.
                 */
            }

            break;
        }
    }

    dd->entry_cluster = cluster_num;
    dd->entry_offset = cluster_offset;

    return arg.finished;
}

/**
 * \ingroup fat_dir
 * Resets a directory handle.
 *
 * Resets the directory handle such that reading restarts
 * with the first directory entry.
 *
 * \param[in] dd The directory handle to reset.
 * \returns 0 on failure, 1 on success.
 * \see fat_read_dir
 */
uint8_t fat_reset_dir(struct fat_dir_struct* dd)
{
    if(!dd)
        return 0;

    dd->entry_cluster = dd->dir_entry.cluster;
    dd->entry_offset = 0;
    return 1;
}

/**
 * \ingroup fat_fs
 * Callback function for reading a directory entry.
 *
 * Interprets a raw directory entry and puts the contained
 * information into a fat_dir_entry_struct structure.
 * 
 * For a single file there may exist multiple directory
 * entries. All except the last one are lfn entries, which
 * contain parts of the long filename. The last directory
 * entry is a traditional 8.3 style one. It contains all
 * other information like size, cluster, date and time.
 * 
 * \param[in] buffer A pointer to 32 bytes of raw data.
 * \param[in] offset The absolute offset of the raw data.
 * \param[in,out] p An argument structure controlling operation.
 * \returns 0 on failure or completion, 1 if reading has
 *          to be continued
 */
uint8_t fat_dir_entry_read_callback(uint8_t* buffer, offset_t offset, void* p)
{
    struct fat_read_dir_callback_arg* arg = p;
    struct fat_dir_entry_struct* dir_entry = arg->dir_entry;

    arg->bytes_read += 32;

    /* skip deleted or empty entries */
    if(buffer[0] == FAT_DIRENTRY_DELETED || !buffer[0])
    {
#if FAT_LFN_SUPPORT
        arg->checksum = 0;
#endif
        return 1;
    }

#if !FAT_LFN_SUPPORT
    /* skip lfn entries */
    if(buffer[11] == 0x0f)
        return 1;
#endif

    char* long_name = dir_entry->long_name;
#if FAT_LFN_SUPPORT
    if(buffer[11] == 0x0f)
    {
        /* checksum validation */
        if(arg->checksum == 0 || arg->checksum != buffer[13])
        {
            /* reset directory entry */
            memset(dir_entry, 0, sizeof(*dir_entry));

            arg->checksum = buffer[13];
            dir_entry->entry_offset = offset;
        }

        /* lfn supports unicode, but we do not, for now.
         * So we assume pure ascii and read only every
         * second byte.
         */
        uint16_t char_offset = ((buffer[0] & 0x3f) - 1) * 13;
        const uint8_t char_mapping[] = { 1, 3, 5, 7, 9, 14, 16, 18, 20, 22, 24, 28, 30 };
        for(uint8_t i = 0; i <= 12 && char_offset + i < sizeof(dir_entry->long_name) - 1; ++i)
            long_name[char_offset + i] = buffer[char_mapping[i]];

        return 1;
    }
    else
#endif
    {
#if FAT_LFN_SUPPORT
        /* if we do not have a long name or the previous lfn does not match, take the 8.3 name */
        if(long_name[0] == '\0' || arg->checksum != fat_calc_83_checksum(buffer))
#endif
        {
            /* reset directory entry */
            memset(dir_entry, 0, sizeof(*dir_entry));
            dir_entry->entry_offset = offset;

            uint8_t i;
            for(i = 0; i < 8; ++i)
            {
                if(buffer[i] == ' ')
                    break;
                long_name[i] = buffer[i];

                /* Windows NT and later versions do not store lfn entries
                 * for 8.3 names which have a lowercase basename, extension
                 * or both when everything else is uppercase. They use two
                 * extra bits to signal a lowercase basename or extension.
                 */
                if((buffer[12] & 0x08) && buffer[i] >= 'A' && buffer[i] <= 'Z')
                    long_name[i] += 'a' - 'A';
            }
            if(long_name[0] == 0x05)
                long_name[0] = (char) FAT_DIRENTRY_DELETED;

            if(buffer[8] != ' ')
            {
                long_name[i++] = '.';

                uint8_t j = 8;
                for(; j < 11; ++j)
                {
                    if(buffer[j] == ' ')
                        break;
                    long_name[i] = buffer[j];

                    /* See above for the lowercase 8.3 name handling of
                     * Windows NT and later.
                     */
                    if((buffer[12] & 0x10) && buffer[j] >= 'A' && buffer[j] <= 'Z')
                        long_name[i] += 'a' - 'A';

                    ++i;
                }
            } 

            long_name[i] = '\0';
        }
        
        /* extract properties of file and store them within the structure */
        dir_entry->attributes = buffer[11];
        dir_entry->cluster = read16(&buffer[26]);
        dir_entry->cluster |= ((cluster_t) read16(&buffer[20])) << 16;
        dir_entry->file_size = read32(&buffer[28]);

#if FAT_DATETIME_SUPPORT
        dir_entry->modification_time = read16(&buffer[22]);
        dir_entry->modification_date = read16(&buffer[24]);
#endif

        arg->finished = 1;
        return 0;
    }
}

#if DOXYGEN || FAT_LFN_SUPPORT
/**
 * \ingroup fat_fs
 * Calculates the checksum for 8.3 names used within the
 * corresponding lfn directory entries.
 *
 * \param[in] file_name_83 The 11-byte file name buffer.
 * \returns The checksum of the given file name.
 */
uint8_t fat_calc_83_checksum(const uint8_t* file_name_83)
{
    uint8_t checksum = file_name_83[0];
    for(uint8_t i = 1; i < 11; ++i)
        checksum = ((checksum >> 1) | (checksum << 7)) + file_name_83[i];

    return checksum;
}
#endif

/**
 * \ingroup fat_dir
 * Deletes a directory.
 *
 * This is just a synonym for fat_delete_file().
 * If a directory is deleted without first deleting its
 * subdirectories and files, disk space occupied by these
 * files will get wasted as there is no chance to release
 * it and mark it as free.
 * 
 * \param[in] fs The filesystem on which to operate.
 * \param[in] dir_entry The directory entry of the directory to delete.
 * \returns 0 on failure, 1 on success.
 * \see fat_create_dir
 */
#ifdef DOXYGEN
uint8_t fat_delete_dir(struct fat_fs_struct* fs, struct fat_dir_entry_struct* dir_entry);
#endif

/**
 * \ingroup fat_dir
 * Moves or renames a directory.
 *
 * This is just a synonym for fat_move_file().
 * 
 * \param[in] fs The filesystem on which to operate.
 * \param[in,out] dir_entry The directory entry of the directory to move.
 * \param[in] parent_new The handle of the new parent directory.
 * \param[in] dir_new The directory's new name.
 * \returns 0 on failure, 1 on success.
 * \see fat_create_dir, fat_delete_dir, fat_move_file
 */
#ifdef DOXYGEN
uint8_t fat_move_dir(struct fat_fs_struct* fs, struct fat_dir_entry_struct* dir_entry, struct fat_dir_struct* parent_new, const char* dir_new);
#endif

#if DOXYGEN || FAT_DATETIME_SUPPORT
/**
 * \ingroup fat_file
 * Returns the modification date of a file.
 *
 * \param[in] dir_entry The directory entry of which to return the modification date.
 * \param[out] year The year the file was last modified.
 * \param[out] month The month the file was last modified.
 * \param[out] day The day the file was last modified.
 */
void fat_get_file_modification_date(const struct fat_dir_entry_struct* dir_entry, uint16_t* year, uint8_t* month, uint8_t* day)
{
    if(!dir_entry)
        return;

    *year = 1980 + ((dir_entry->modification_date >> 9) & 0x7f);
    *month = (dir_entry->modification_date >> 5) & 0x0f;
    *day = (dir_entry->modification_date >> 0) & 0x1f;
}
#endif

#if DOXYGEN || FAT_DATETIME_SUPPORT
/**
 * \ingroup fat_file
 * Returns the modification time of a file.
 *
 * \param[in] dir_entry The directory entry of which to return the modification time.
 * \param[out] hour The hour the file was last modified.
 * \param[out] min The min the file was last modified.
 * \param[out] sec The sec the file was last modified.
 */
void fat_get_file_modification_time(const struct fat_dir_entry_struct* dir_entry, uint8_t* hour, uint8_t* min, uint8_t* sec)
{
    if(!dir_entry)
        return;

    *hour = (dir_entry->modification_time >> 11) & 0x1f;
    *min = (dir_entry->modification_time >> 5) & 0x3f;
    *sec = ((dir_entry->modification_time >> 0) & 0x1f) * 2;
}
#endif

/**
 * \ingroup fat_fs
 * Returns the amount of total storage capacity of the filesystem in bytes.
 *
 * \param[in] fs The filesystem on which to operate.
 * \returns 0 on failure, the filesystem size in bytes otherwise.
 */
offset_t fat_get_fs_size(const struct fat_fs_struct* fs)
{
    if(!fs)
        return 0;

    if(fs->partition->type == PARTITION_TYPE_FAT32)
        return (offset_t) (fs->header.fat_size / 4 - 2) * fs->header.cluster_size;
    else
        return (offset_t) (fs->header.fat_size / 2 - 2) * fs->header.cluster_size;
}

/**
 * \ingroup fat_fs
 * Returns the amount of free storage capacity on the filesystem in bytes.
 *
 * \note As the FAT filesystem is cluster based, this function does not
 *       return continuous values but multiples of the cluster size.
 *
 * \param[in] fs The filesystem on which to operate.
 * \returns 0 on failure, the free filesystem space in bytes otherwise.
 */
offset_t fat_get_fs_free(const struct fat_fs_struct* fs)
{
    if(!fs)
        return 0;

    uint8_t fat[32];
    struct fat_usage_count_callback_arg count_arg;
    count_arg.cluster_count = 0;
    count_arg.buffer_size = sizeof(fat);

    offset_t fat_offset = fs->header.fat_offset;
    uint32_t fat_size = fs->header.fat_size;
    while(fat_size > 0)
    {
        uintptr_t length = UINTPTR_MAX - 1;
        if(fat_size < length)
            length = fat_size;

        if(!sd_raw_read_interval(fat_offset,
                                fat,
                                sizeof(fat),
                                length,
                                (fs->partition->type == PARTITION_TYPE_FAT16) ?
                                    fat_get_fs_free_16_callback :
                                    fat_get_fs_free_32_callback,
                                &count_arg
                               )
          )
            return 0;

        fat_offset += length;
        fat_size -= length;
    }

    return (offset_t) count_arg.cluster_count * fs->header.cluster_size;
}

/**
 * \ingroup fat_fs
 * Callback function used for counting free clusters in a FAT.
 */
uint8_t fat_get_fs_free_16_callback(uint8_t* buffer, offset_t offset, void* p)
{
    struct fat_usage_count_callback_arg* count_arg = (struct fat_usage_count_callback_arg*) p;
    uintptr_t buffer_size = count_arg->buffer_size;

    for(uintptr_t i = 0; i < buffer_size; i += 2, buffer += 2)
    {
        uint16_t cluster = read16(buffer);
        if(cluster == HTOL16(FAT16_CLUSTER_FREE))
            ++(count_arg->cluster_count);
    }

    return 1;
}

/**
 * \ingroup fat_fs
 * Callback function used for counting free clusters in a FAT32.
 */
uint8_t fat_get_fs_free_32_callback(uint8_t* buffer, offset_t offset, void* p)
{
    struct fat_usage_count_callback_arg* count_arg = (struct fat_usage_count_callback_arg*) p;
    uintptr_t buffer_size = count_arg->buffer_size;

    for(uintptr_t i = 0; i < buffer_size; i += 4, buffer += 4)
    {
        uint32_t cluster = read32(buffer);
        if(cluster == HTOL32(FAT32_CLUSTER_FREE))
            ++(count_arg->cluster_count);
    }

    return 1;
}
