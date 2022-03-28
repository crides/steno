/*
 * The MIT License (MIT)
 *
 * Copyright (c) Microsoft Corporation
 * Copyright (c) 2020 Ha Thach for Adafruit Industries
 * Copyright (c) 2020 Henry Gabryjelski
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "compile_date.h"
#include "scsi.h"
#include "store.h"
#include "steno.h"

#include <string.h>
#include <stdio.h>

static struct TextFile const info[] = {
    // current.uf2 must be the last element and its content must be NULL
    {.name = "STENO   BIN", .content = NULL},
};

static FAT_BootBlock const BootBlock = {
    .JumpInstruction = {0xeb, 0x3c, 0x90},
    .OEMInfo = "UF2 UF2 ",
    .SectorSize = BLOCK_SIZE,
    .SectorsPerCluster = BLOCKS_PER_CLUSTER,
    .ReservedSectors = RESERVED_BLOCKS,
    .FATCopies = NUMBER_OF_FATS,
    .RootDirectoryEntries = ROOT_DIR_ENTRIES,
    /* .TotalSectors16 = (FS_TOTAL_BLOCKS > 0xFFFF) ? 0 : FS_TOTAL_BLOCKS, */
    .TotalSectors16 = 0,
    .MediaDescriptor = MEDIA_DESCRIPTOR_BYTE,
    .SectorsPerFAT = TABLE_BLOCKS,
    .SectorsPerTrack = 1,
    .Heads = 1,
    /* .TotalSectors32 = (FS_TOTAL_BLOCKS > 0xFFFF) ? FS_TOTAL_BLOCKS : 0, */
    .TotalSectors32 = TOTAL_CLUSTERS * BLOCKS_PER_CLUSTER,
    .PhysicalDriveNum = 0x80, // to match MediaDescriptor of 0xF8
    .ExtendedBootSig = 0x29,
    .VolumeSerialNumber = 0xfeed6062,
    .VolumeLabel = "BATWINGS   ",
    .FilesystemIdentifier = "FAT16   ",
};

void padded_memcpy(char *dst, char const *src, const int len) {
    for (int i = 0; i < len; ++i) {
        if (*src) {
            *dst = *src++;
        } else {
            *dst = ' ';
        }
        dst++;
    }
}

// `data` will be packet sized
void fat_read_block(const uint32_t block_no, const uint8_t packet_num, uint8_t *const data) {
    uint32_t cluster_no = block_no / BLOCKS_PER_CLUSTER;
    const uint8_t cluster_packet_num = packet_num + ((block_no % BLOCKS_PER_CLUSTER) ? PACKETS_PER_BLOCK : 0);
    memset(data, 0, EPSIZE);
    if (cluster_no == 0) { // Requested boot block
        if (cluster_packet_num == 0) {
            memcpy(data, &BootBlock, sizeof(BootBlock));
        } else if (cluster_packet_num == 7) {
            data[62] = 0x55; // Always at offsets 510/511, even when BLOCK_SIZE is larger
            data[63] = 0xaa; // Always at offsets 510/511, even when BLOCK_SIZE is larger
        }
    } else if (cluster_no < ROOTDIR_START) { // Requested FAT table sector
        if (cluster_no >= FAT1_START) {
            cluster_no -= FAT1_START;
        } else {
            cluster_no -= FAT0_START;
        }
        if (cluster_no == 0 && cluster_packet_num == 0) {
            // first FAT entry must match BPB MediaDescriptor
            data[0] = MEDIA_DESCRIPTOR_BYTE;
            data[1] = 0xff;
            data[2] = 0xff;
            data[3] = 0xff;
            // WARNING -- code presumes only one NULL .content for .UF2 file
            //            and all non-NULL .content fit in one sector
            //            and requires it be the last element of the array
        }
        for (uint32_t i = 0; i < FAT_ENTRIES_PER_PACKET; ++i) { // Generate the FAT chain for the firmware "file"
            const uint16_t v = (cluster_no * FAT_ENTRIES_PER_CLUSTER) + cluster_packet_num * FAT_ENTRIES_PER_PACKET + i;
            if (v >= FILE_FIRST_DATA_CLUSTER) {
                if (v < FILE_LAST_DATA_CLUSTER) {
                    ((uint16_t *)(void *)data)[i] = v + 1;
                } else if (v == FILE_LAST_DATA_CLUSTER) {
                    ((uint16_t *)(void *)data)[i] = 0xffff;
                }
            }
        }
    } else if (cluster_no < FILE_START) { // Requested root directory sector
        cluster_no -= ROOTDIR_START;
        DirEntry *d = (void *) data;
        // Since we only have 2 entries (the volume and the firmware), and they also fit inside one
        // packet, we can construct this manually
        if (cluster_no == 0 && cluster_packet_num == 0) { // volume label first
            // volume label is first directory entry
            padded_memcpy(d[0].name, (char const *) BootBlock.VolumeLabel, 11);
            d[0].attrs = 0x28;

            struct TextFile const *inf = &info[0];
            padded_memcpy(d[1].name, inf->name, 11);
            d[1].createTimeFine = __SECONDS_INT__ % 2 * 100;
            d[1].createTime = __DOSTIME__;
            d[1].createDate = __DOSDATE__;
            d[1].lastAccessDate = __DOSDATE__;
            d[1].highStartCluster = 0;
            // DIR_WrtTime and DIR_WrtDate must be supported
            d[1].updateTime = __DOSTIME__;
            d[1].updateDate = __DOSDATE__;
            d[1].startCluster = 2;
            d[1].size = FILE_SIZE;
        }
    } else if (cluster_no < FILE_END) {
        cluster_no -= FILE_START;
        store_read((cluster_no * PACKETS_PER_CLUSTER + cluster_packet_num) * EPSIZE, data, EPSIZE);
    }
}
