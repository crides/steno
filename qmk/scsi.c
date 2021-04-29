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
 *  SCSI command processing routines, for SCSI commands issued by the host. Mass Storage
 *  devices use a thin "Bulk-Only Transport" protocol for issuing commands and status information,
 *  which wrap around standard SCSI device commands for controlling the actual storage medium.
 */

#include "scsi.h"
#include "store.h"
#include "stroke.h"
#include "dict_editing.h"

static bool scsi_inquiry(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo);
static bool scsi_request_sense(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo);
static bool scsi_read_capacity_10(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo);
static bool scsi_send_diagnostic(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo);
static bool scsi_read_write_10(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo, const bool IsDataRead);
static bool scsi_mode_sense_6(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo);

static uint8_t packet_buf[EPSIZE];
/* static uint8_t uf2_header[32]; */

/** Structure to hold the SCSI response data to a SCSI INQUIRY command. This gives information about the device's
 *  features and capabilities.
 */
static const SCSI_Inquiry_Response_t inquiry_data = {
    .DeviceType = DEVICE_TYPE_BLOCK,
    .PeripheralQualifier = 0,

    .Removable = true,
    .Version = 0,
    .ResponseDataFormat = 2,
    .NormACA = false,
    .TrmTsk = false,
    .AERC = false,

    .AdditionalLength = 0x1F,

    .SoftReset = false,
    .CmdQue = false,
    .Linked = false,
    .Sync = false,
    .WideBus16Bit = false,
    .WideBus32Bit = false,
    .RelAddr = false,

    .VendorID = "Crides",
    .ProductID = "Batwing Steno",
    .RevisionID = {'0', '.', '0', '0'},
};

/** Structure to hold the sense data for the last issued SCSI command, which is returned to the host after a SCSI
 * REQUEST SENSE command is issued. This gives information on exactly why the last command failed to complete.
 */
static SCSI_Request_Sense_Response_t SenseData = {
    .ResponseCode = 0x70,
    .AdditionalLength = 0x0A,
};

/** Main routine to process the SCSI command located in the Command Block Wrapper read from the host. This dispatches
 *  to the appropriate SCSI command handling routine if the issued command is supported by the device, else it returns
 *  a command failure due to a ILLEGAL REQUEST.
 */
bool handle_scsi_command(USB_ClassInfo_MS_Device_t *const msc_interface_info) {
    bool success = false;

    /* Run the appropriate SCSI command hander function based on the passed command */
    switch (msc_interface_info->State.CommandBlock.SCSICommandData[0]) {
    case SCSI_CMD_INQUIRY:
        success = scsi_inquiry(msc_interface_info);
        break;
    case SCSI_CMD_REQUEST_SENSE:
        success = scsi_request_sense(msc_interface_info);
        break;
    case SCSI_CMD_READ_CAPACITY_10:
        success = scsi_read_capacity_10(msc_interface_info);
        break;
    case SCSI_CMD_SEND_DIAGNOSTIC:
        success = scsi_send_diagnostic(msc_interface_info);
        break;
    case SCSI_CMD_WRITE_10:
        success = scsi_read_write_10(msc_interface_info, DATA_WRITE);
        break;
    case SCSI_CMD_READ_10:
        success = scsi_read_write_10(msc_interface_info, DATA_READ);
        break;
    case SCSI_CMD_MODE_SENSE_6:
        success = scsi_mode_sense_6(msc_interface_info);
        break;
    case SCSI_CMD_START_STOP_UNIT:
    case SCSI_CMD_TEST_UNIT_READY:
    case SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:
    case SCSI_CMD_VERIFY_10:
        /* These commands should just succeed, no handling required */
        success = true;
        msc_interface_info->State.CommandBlock.DataTransferLength = 0;
        break;
    default:
        /* Update the SENSE key to reflect the invalid command */
        SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASENSE_INVALID_COMMAND, SCSI_ASENSEQ_NO_QUALIFIER);
        break;
    }

    /* Check if command was successfully processed */
    if (success) {
        SCSI_SET_SENSE(SCSI_SENSE_KEY_GOOD, SCSI_ASENSE_NO_ADDITIONAL_INFORMATION, SCSI_ASENSEQ_NO_QUALIFIER);
    }
    return success;
}

void scsi_write(USB_ClassInfo_MS_Device_t *const msc_interface_info, const uint32_t block_addr, uint16_t blocks) {
    if (Endpoint_WaitUntilReady()) {
        return;
    }
    if (!(Endpoint_IsReadWriteAllowed())) {
        Endpoint_ClearOUT();
        if (Endpoint_WaitUntilReady()) {
            return;
        }
    }
    for ( ; blocks > 0; blocks --) {
        uint8_t _header[32];
        uint8_t data_buf[256];
        Endpoint_Read_Stream_LE(_header, 32, NULL);
        const uint32_t *const header = (uint32_t *) _header;
        Endpoint_Read_Stream_LE(data_buf, sizeof(data_buf), NULL);
        Endpoint_Discard_Stream(512 - 256 - 32 - sizeof(uint32_t), NULL);
        uint32_t last_word;
        Endpoint_Read_Stream_LE(&last_word, sizeof(uint32_t), NULL);
        const bool valid_header = (header[0] == UF2_MAGIC0 && header[1] == UF2_MAGIC1 && last_word == UF2_MAGIC_END
                && (header[2] & UF2_FLAG_FAMILYID) && (!(header[2] & UF2_FLAG_NOFLASH))
                && ((header[3] & 0xFF) == 0) && header[4] == 256 && header[7] == UF2_FAMILY_ID);
        if (valid_header) {
            if (header[5] == 0) {
                steno_error_ln("erase");
                store_rewrite_start();
                steno_error_ln("flash");
            }
            store_rewrite_write(header[3], data_buf);
        }
        if (msc_interface_info->State.IsMassStoreReset) {
            steno_error_ln("reset");
            return;
        }

        if (!(Endpoint_IsReadWriteAllowed())) {
            Endpoint_ClearOUT();
        }
    }
}

void scsi_read(USB_ClassInfo_MS_Device_t *const msc_interface_info, const uint32_t block_addr, uint16_t blocks) {
    if (Endpoint_WaitUntilReady()) {
        return;
    }
    for (uint16_t i = 0; i < blocks; i ++) {
        for (uint8_t packet_num = 0; packet_num < 8; packet_num ++) {
            if (!(Endpoint_IsReadWriteAllowed())) {
                Endpoint_ClearIN();
                if (Endpoint_WaitUntilReady()) {
                    return;
                }
            }
            fat_read_block(block_addr + i, packet_num, (uint8_t *) packet_buf);
            Endpoint_Write_Stream_LE(packet_buf, EPSIZE, NULL);
            if (msc_interface_info->State.IsMassStoreReset) {
                return;
            }
        }
    }
    if (!(Endpoint_IsReadWriteAllowed())) {
        Endpoint_ClearIN();
    }
}

/** Command processing for an issued SCSI INQUIRY command. This command returns information about the device's features
 *  and capabilities to the host.
 */
static bool scsi_inquiry(USB_ClassInfo_MS_Device_t *const msc_interface_info) {
    const uint16_t AllocationLength = SwapEndian_16(*(uint16_t *) &msc_interface_info->State.CommandBlock.SCSICommandData[3]);
    const uint16_t BytesTransferred = MIN(AllocationLength, sizeof(inquiry_data));

    /* Only the standard INQUIRY data is supported, check if any optional INQUIRY bits set */
    if ((msc_interface_info->State.CommandBlock.SCSICommandData[1] & ((1 << 0) | (1 << 1))) ||
        msc_interface_info->State.CommandBlock.SCSICommandData[2]) {
        /* Optional but unsupported bits set - update the SENSE key and fail the request */
        SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASENSE_INVALID_FIELD_IN_CDB, SCSI_ASENSEQ_NO_QUALIFIER);
        return false;
    }

    Endpoint_Write_Stream_LE(&inquiry_data, BytesTransferred, NULL);

    /* Pad out remaining bytes with 0x00 */
    Endpoint_Null_Stream((AllocationLength - BytesTransferred), NULL);

    /* Finalize the stream transfer to send the last packet */
    Endpoint_ClearIN();

    /* Succeed the command and update the bytes transferred counter */
    msc_interface_info->State.CommandBlock.DataTransferLength -= BytesTransferred;

    return true;
}

/** Command processing for an issued SCSI REQUEST SENSE command. This command returns information about the last issued
 * command, including the error code and additional error information so that the host can determine why a command
 * failed to complete.
 */
static bool scsi_request_sense(USB_ClassInfo_MS_Device_t *const msc_interface_info) {
    const uint8_t AllocationLength = msc_interface_info->State.CommandBlock.SCSICommandData[4];
    const uint8_t BytesTransferred = MIN(AllocationLength, sizeof(SenseData));

    Endpoint_Write_Stream_LE(&SenseData, BytesTransferred, NULL);
    Endpoint_Null_Stream((AllocationLength - BytesTransferred), NULL);
    Endpoint_ClearIN();

    /* Succeed the command and update the bytes transferred counter */
    msc_interface_info->State.CommandBlock.DataTransferLength -= BytesTransferred;

    return true;
}

/** Command processing for an issued SCSI READ CAPACITY (10) command. This command returns information about the
 * device's capacity on the selected Logical Unit (drive), as a number of OS-sized blocks.
 */
static bool scsi_read_capacity_10(USB_ClassInfo_MS_Device_t *const msc_interface_info) {
    const uint32_t LastBlockAddressInLUN = TOTAL_BLOCKS - 1;
    const uint32_t MediaBlockSize = BLOCK_SIZE;

    Endpoint_Write_Stream_BE(&LastBlockAddressInLUN, sizeof(LastBlockAddressInLUN), NULL);
    Endpoint_Write_Stream_BE(&MediaBlockSize, sizeof(MediaBlockSize), NULL);
    Endpoint_ClearIN();

    /* Succeed the command and update the bytes transferred counter */
    msc_interface_info->State.CommandBlock.DataTransferLength -= 8;

    return true;
}

/** Command processing for an issued SCSI SEND DIAGNOSTIC command. This command performs a quick check of the Dataflash
 * ICs on the board, and indicates if they are present and functioning correctly. Only the Self-Test portion of the
 * diagnostic command is supported.
 */
static bool scsi_send_diagnostic(USB_ClassInfo_MS_Device_t *const msc_interface_info) {
    /* Check to see if the SELF TEST bit is not set */
    if (!(msc_interface_info->State.CommandBlock.SCSICommandData[1] & (1 << 2))) {
        /* Only self-test supported - update SENSE key and fail the command */
        SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASENSE_INVALID_FIELD_IN_CDB, SCSI_ASENSEQ_NO_QUALIFIER);

        return false;
    }

    /* Succeed the command and update the bytes transferred counter */
    msc_interface_info->State.CommandBlock.DataTransferLength = 0;

    return true;
}

/** Command processing for an issued SCSI READ (10) or WRITE (10) command. This command reads in the block start address
 *  and total number of blocks to process, then calls the appropriate low-level Dataflash routine to handle the actual
 *  reading and writing of the data.
 */
static bool scsi_read_write_10(USB_ClassInfo_MS_Device_t *const msc_interface_info, const bool IsDataRead) {
    uint32_t block_addr;
    uint16_t blocks;

    /* Check if the disk is write protected or not */
    if ((IsDataRead == DATA_WRITE) && DISK_READ_ONLY) {
        SCSI_SET_SENSE(SCSI_SENSE_KEY_DATA_PROTECT, SCSI_ASENSE_WRITE_PROTECTED, SCSI_ASENSEQ_NO_QUALIFIER);
        return false;
    }

    /* Load in the 32-bit block address (SCSI uses big-endian, so have to reverse the byte order) */
    block_addr = SwapEndian_32(*(uint32_t *) &msc_interface_info->State.CommandBlock.SCSICommandData[2]);

    /* Load in the 16-bit total blocks (SCSI uses big-endian, so have to reverse the byte order) */
    blocks = SwapEndian_16(*(uint16_t *) &msc_interface_info->State.CommandBlock.SCSICommandData[7]);

    /* Check if the block address is outside the maximum allowable value for the LUN */
    if (block_addr >= TOTAL_BLOCKS) {
        /* Block address is invalid, update SENSE key and return command fail */
        SCSI_SET_SENSE(SCSI_SENSE_KEY_ILLEGAL_REQUEST, SCSI_ASENSE_LOGICAL_BLOCK_ADDRESS_OUT_OF_RANGE,
                       SCSI_ASENSEQ_NO_QUALIFIER);
        return false;
    }

    /* Determine if the packet is a READ (10) or WRITE (10) command, call appropriate function */
    if (IsDataRead == DATA_READ) {
        scsi_read(msc_interface_info, block_addr, blocks);
    } else {
        scsi_write(msc_interface_info, block_addr, blocks);
    }

    /* Update the bytes transferred counter and succeed the command */
    msc_interface_info->State.CommandBlock.DataTransferLength -= ((uint32_t) blocks * BLOCK_SIZE);

    return true;
}

/** Command processing for an issued SCSI MODE SENSE (6) command. This command returns various informational pages about
 *  the SCSI device, as well as the device's Write Protect status.
 */
static bool scsi_mode_sense_6(USB_ClassInfo_MS_Device_t *const msc_interface_info) {
    /* Send an empty header response with the Write Protect flag status */
    Endpoint_Write_8(0x00);
    Endpoint_Write_8(0x00);
    Endpoint_Write_8(DISK_READ_ONLY ? 0x80 : 0x00);
    Endpoint_Write_8(0x00);
    Endpoint_ClearIN();

    /* Update the bytes transferred counter and succeed the command */
    msc_interface_info->State.CommandBlock.DataTransferLength -= 4;

    return true;
}
