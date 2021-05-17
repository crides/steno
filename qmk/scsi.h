/*
             LUFA Library
     Copyright (C) Dean Camera, 2019.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2019  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Header file for SCSI.c.
 */

#pragma once 

#include <LUFA/Drivers/USB/USB.h>

#define STATIC_ASSERT(thing) _Static_assert(thing, "static assert")

/** Macro to set the current SCSI sense data to the given key, additional sense code and additional sense qualifier.
 * This is for convenience, as it allows for all three sense values (returned upon request to the host to give
 * information about the last command failure) in a quick and easy manner.
 */
#define SCSI_SET_SENSE(Key, Acode, Aqual)                                                                              \
    do {                                                                                                               \
        SenseData.SenseKey = (Key);                                                                                    \
        SenseData.AdditionalSenseCode = (Acode);                                                                       \
        SenseData.AdditionalSenseQualifier = (Aqual);                                                                  \
    } while (0)

/** Macro for the \ref scsi_ReadWrite_10() function, to indicate that data is to be read from the storage
 * medium. */
#define DATA_READ true

/** Macro for the \ref scsi_ReadWrite_10() function, to indicate that data is to be written to the storage
 * medium. */
#define DATA_WRITE false
#define DEVICE_TYPE_BLOCK 0x00

typedef struct {
    uint8_t JumpInstruction[3];
    uint8_t OEMInfo[8];
    uint16_t SectorSize;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t FATCopies;
    uint16_t RootDirectoryEntries;
    uint16_t TotalSectors16;
    uint8_t MediaDescriptor;
    uint16_t SectorsPerFAT;
    uint16_t SectorsPerTrack;
    uint16_t Heads;
    uint32_t HiddenSectors;
    uint32_t TotalSectors32;
    uint8_t PhysicalDriveNum;
    uint8_t Reserved;
    uint8_t ExtendedBootSig;
    uint32_t VolumeSerialNumber;
    uint8_t VolumeLabel[11];
    uint8_t FilesystemIdentifier[8];
} __attribute__((packed)) FAT_BootBlock;

typedef struct {
    char name[8];
    char ext[3];
    uint8_t attrs;
    uint8_t reserved;
    uint8_t createTimeFine;
    uint16_t createTime;
    uint16_t createDate;
    uint16_t lastAccessDate;
    uint16_t highStartCluster;
    uint16_t updateTime;
    uint16_t updateDate;
    uint16_t startCluster;
    uint32_t size;
} __attribute__((packed)) DirEntry;

struct TextFile {
    char const name[11];
    char const *content;
};

typedef struct {
    uint32_t magic0;
    uint32_t magic1;
    uint32_t flags;
    uint32_t target_addr;
    uint32_t payload_size;
    uint32_t block_no;
    uint32_t block_num;
    uint32_t family_id;
} uf2_header_t;

/* Function Prototypes: */
bool handle_scsi_command(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo);
void fat_read_block(uint32_t block_no, uint8_t packet_num, uint8_t *data);
void fat_write_block(uint32_t block_no, uint8_t packet_num, uint8_t *data);

#define UF2_MAGIC0 0x0A324655
#define UF2_MAGIC1 0x9E5D5157
#define UF2_MAGIC_END 0x0AB16F30
// Family ID present
#define UF2_FLAG_FAMILYID 0x00002000
#define UF2_FLAG_NOFLASH 0x00000001
// STOEUPB
#define UF2_FAMILY_ID 0x00302cc0
#define UF2_DATA_SIZE 476
#define DATA_SIZE 256

#define FILE_SIZE (16ul << 20) // 16MB
#define UF2_FLASH_SIZE (32ul << 20)      // 256 data blocks wrapped in 512 byte blocks
#define DATA_CLUSTERS_SIZE (FILE_SIZE + UF2_FLASH_SIZE)
#define BLOCK_SIZE 512  // GhostFAT does not support other sector sizes (currently) */
#define DISK_READ_ONLY false
#define EPSIZE 64

#define BLOCKS_PER_CLUSTER (2)
#define RESERVED_CLUSTERS (1)
#define RESERVED_BLOCKS (RESERVED_CLUSTERS * BLOCKS_PER_CLUSTER)
#define NUMBER_OF_FATS (2) // FAT highest compatibility
#define MEDIA_DESCRIPTOR_BYTE (0xF8)
#define FAT_ENTRY_SIZE (2) // FAT16
#define FAT_ENTRIES_PER_BLOCK (BLOCK_SIZE / FAT_ENTRY_SIZE)
#define FAT_ENTRIES_PER_CLUSTER (FAT_ENTRIES_PER_BLOCK * BLOCKS_PER_CLUSTER)
#define FAT_ENTRIES_PER_PACKET (EPSIZE / FAT_ENTRY_SIZE)
#define NUM_FILES 1
#define ROOT_DIR_CLUSTERS 1
#define PACKETS_PER_BLOCK (BLOCK_SIZE / EPSIZE)
#define PACKETS_PER_CLUSTER (PACKETS_PER_BLOCK * BLOCKS_PER_CLUSTER)

#define FILE_BLOCKS (FILE_SIZE / BLOCK_SIZE)
#define FILE_CLUSTERS (FILE_BLOCKS / BLOCKS_PER_CLUSTER)
#define UF2_FLASH_CLUSTERS (UF2_FLASH_SIZE / BLOCK_SIZE / BLOCKS_PER_CLUSTER)
// HACK to make fsck.fat happy
#define DATA_CLUSTERS ((DATA_CLUSTERS_SIZE / BLOCKS_PER_CLUSTER / BLOCK_SIZE))
// NOTE: MS specification explicitly allows FAT to be larger than necessary
// Number of blocks used by each cluster map
#define TABLE_CLUSTERS ((DATA_CLUSTERS / FAT_ENTRIES_PER_CLUSTER) + ((DATA_CLUSTERS % FAT_ENTRIES_PER_CLUSTER) ? 1 : 0))
/* #define TABLE_BLOCKS ((DATA_CLUSTERS / FAT_ENTRIES_PER_BLOCK) + ((DATA_CLUSTERS % FAT_ENTRIES_PER_BLOCK) ? 1 : 0)) */
#define TABLE_BLOCKS (TABLE_CLUSTERS * BLOCKS_PER_CLUSTER)
#define DIRENTRIES_PER_BLOCK (BLOCK_SIZE / sizeof(DirEntry))
#define ROOT_DIR_ENTRIES (ROOT_DIR_CLUSTERS * DIRENTRIES_PER_BLOCK * BLOCKS_PER_CLUSTER)

#define FILE_FIRST_DATA_CLUSTER (ROOT_DIR_CLUSTERS + 1)
// NOTE This is the last cluster, not the one after the last
#define FILE_LAST_DATA_CLUSTER (FILE_FIRST_DATA_CLUSTER + FILE_CLUSTERS - 1)

#define FAT0_START RESERVED_CLUSTERS
#define FAT1_START (FAT0_START + TABLE_CLUSTERS)
#define ROOTDIR_START (FAT1_START + TABLE_CLUSTERS)
#define FILE_START (ROOTDIR_START + ROOT_DIR_CLUSTERS)
#define FILE_END (FILE_START + FILE_CLUSTERS)
// HACK to make fsck.fat happy
#define TOTAL_CLUSTERS (FILE_END + UF2_FLASH_CLUSTERS - 2)
#define TOTAL_BLOCKS (TOTAL_CLUSTERS * BLOCKS_PER_CLUSTER)

STATIC_ASSERT(sizeof(DirEntry) == 32);
STATIC_ASSERT(FILE_CLUSTERS == 16ul << 10);
// NOTE What mkfs.msdos seem to have done
STATIC_ASSERT(DATA_CLUSTERS == (48ul << 10));
STATIC_ASSERT(TABLE_CLUSTERS == 96);
STATIC_ASSERT(DATA_CLUSTERS_SIZE == 48ul << 20);
