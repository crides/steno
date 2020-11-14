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

#ifndef _SCSI_H_
#define _SCSI_H_

#include <LUFA/Drivers/USB/USB.h>

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

#if defined(INCLUDE_FROM_SCSI_C)
static bool scsi_inquiry(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo);
static bool scsi_request_sense(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo);
static bool scsi_read_capacity_10(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo);
static bool scsi_send_diagnostic(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo);
static bool scsi_read_write_10(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo, const bool IsDataRead);
static bool scsi_mode_sense_6(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo);
#endif

#define FLASH_SIZE (16 * (1ul << 20)) // 16MB
#define BLOCK_SIZE 512  // GhostFAT does not support other sector sizes (currently) */
#define FLASH_BLOCKS (FLASH_SIZE / BLOCK_SIZE)
#define DISK_READ_ONLY false
#define EPSIZE 64

#define BLOCKS_PER_CLUSTER (1) // GhostFAT presumes one sector == one cluster (for simplicity)
#define RESERVED_BLOCKS (1)
#define NUMBER_OF_FATS (2) // FAT highest compatibility
#define ROOT_DIR_ENTRIES (16)
#define MEDIA_DESCRIPTOR_BYTE (0xF8)
#define FAT_ENTRY_SIZE (2) // FAT16
#define FAT_ENTRIES_PER_BLOCK (BLOCK_SIZE / FAT_ENTRY_SIZE)
#define FAT_ENTRIES_PER_PACKET (EPSIZE / FAT_ENTRY_SIZE)
#define FAT_BLOCKS (FLASH_BLOCKS + NUM_DIRENTRIES) // Flash blocks + 1 reserved block + ID
// NOTE: MS specification explicitly allows FAT to be larger than necessary
// Number of blocks used by each cluster map
#define CLUSTER_BLOCKS ((FAT_BLOCKS / FAT_ENTRIES_PER_BLOCK) + ((FAT_BLOCKS % FAT_ENTRIES_PER_BLOCK) ? 1 : 0))
#define DIRENTRIES_PER_BLOCK (BLOCK_SIZE / sizeof(DirEntry))
#define ROOT_DIR_BLOCKS (ROOT_DIR_ENTRIES / DIRENTRIES_PER_BLOCK)

#define NUM_FILES 1
#define NUM_DIRENTRIES (NUM_FILES + 1) // Root directory + files

#define FLASH_FIRST_BLOCK (NUM_DIRENTRIES * BLOCKS_PER_CLUSTER)
#define FLASH_LAST_BLOCK ((FLASH_FIRST_BLOCK + FLASH_BLOCKS - 1) * BLOCKS_PER_CLUSTER)

#define FS_FAT0 RESERVED_BLOCKS
#define FS_FAT1 (FS_FAT0 + CLUSTER_BLOCKS)
#define FS_ROOTDIR (FS_FAT1 + CLUSTER_BLOCKS)
#define FS_DATA_BLOCKS (FS_ROOTDIR + ROOT_DIR_BLOCKS)

#define FS_BLOCKS (FS_DATA_BLOCKS + FLASH_BLOCKS)

#endif
