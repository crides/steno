/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2014      */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include <string.h>
#include <avr/io.h>
#include "steno.h"

/* commands available in SPI mode */

/* CMD0: response R1 */
#define CMD_GO_IDLE_STATE 0x00
/* CMD1: response R1 */
#define CMD_SEND_OP_COND 0x01
/* CMD8: response R7 */
#define CMD_SEND_IF_COND 0x08
/* CMD9: response R1 */
#define CMD_SEND_CSD 0x09
/* CMD10: response R1 */
#define CMD_SEND_CID 0x0a
/* CMD12: response R1 */
#define CMD_STOP_TRANSMISSION 0x0c
/* CMD13: response R2 */
#define CMD_SEND_STATUS 0x0d
/* CMD16: arg0[31:0]: block length, response R1 */
#define CMD_SET_BLOCKLEN 0x10
/* CMD17: arg0[31:0]: data address, response R1 */
#define CMD_READ_SINGLE_BLOCK 0x11
/* CMD18: arg0[31:0]: data address, response R1 */
#define CMD_READ_MULTIPLE_BLOCK 0x12
/* CMD24: arg0[31:0]: data address, response R1 */
#define CMD_WRITE_SINGLE_BLOCK 0x18
/* CMD25: arg0[31:0]: data address, response R1 */
#define CMD_WRITE_MULTIPLE_BLOCK 0x19
/* CMD27: response R1 */
#define CMD_PROGRAM_CSD 0x1b
/* CMD28: arg0[31:0]: data address, response R1b */
#define CMD_SET_WRITE_PROT 0x1c
/* CMD29: arg0[31:0]: data address, response R1b */
#define CMD_CLR_WRITE_PROT 0x1d
/* CMD30: arg0[31:0]: write protect data address, response R1 */
#define CMD_SEND_WRITE_PROT 0x1e
/* CMD32: arg0[31:0]: data address, response R1 */
#define CMD_TAG_SECTOR_START 0x20
/* CMD33: arg0[31:0]: data address, response R1 */
#define CMD_TAG_SECTOR_END 0x21
/* CMD34: arg0[31:0]: data address, response R1 */
#define CMD_UNTAG_SECTOR 0x22
/* CMD35: arg0[31:0]: data address, response R1 */
#define CMD_TAG_ERASE_GROUP_START 0x23
/* CMD36: arg0[31:0]: data address, response R1 */
#define CMD_TAG_ERASE_GROUP_END 0x24
/* CMD37: arg0[31:0]: data address, response R1 */
#define CMD_UNTAG_ERASE_GROUP 0x25
/* CMD38: arg0[31:0]: stuff bits, response R1b */
#define CMD_ERASE 0x26
/* ACMD41: arg0[31:0]: OCR contents, response R1 */
#define CMD_SD_SEND_OP_COND 0x29
/* CMD42: arg0[31:0]: stuff bits, response R1b */
#define CMD_LOCK_UNLOCK 0x2a
/* CMD55: arg0[31:0]: stuff bits, response R1 */
#define CMD_APP 0x37
/* CMD58: arg0[31:0]: stuff bits, response R3 */
#define CMD_READ_OCR 0x3a
/* CMD59: arg0[31:1]: stuff bits, arg0[0:0]: crc option, response R1 */
#define CMD_CRC_ON_OFF 0x3b

/* command responses */
/* R1: size 1 byte */
#define R1_IDLE_STATE 0
#define R1_ERASE_RESET 1
#define R1_ILL_COMMAND 2
#define R1_COM_CRC_ERR 3
#define R1_ERASE_SEQ_ERR 4
#define R1_ADDR_ERR 5
#define R1_PARAM_ERR 6
/* R1b: equals R1, additional busy bytes */
/* R2: size 2 bytes */
#define R2_CARD_LOCKED 0
#define R2_WP_ERASE_SKIP 1
#define R2_ERR 2
#define R2_CARD_ERR 3
#define R2_CARD_ECC_FAIL 4
#define R2_WP_VIOLATION 5
#define R2_INVAL_ERASE 6
#define R2_OUT_OF_RANGE 7
#define R2_CSD_OVERWRITE 7
#define R2_IDLE_STATE (R1_IDLE_STATE + 8)
#define R2_ERASE_RESET (R1_ERASE_RESET + 8)
#define R2_ILL_COMMAND (R1_ILL_COMMAND + 8)
#define R2_COM_CRC_ERR (R1_COM_CRC_ERR + 8)
#define R2_ERASE_SEQ_ERR (R1_ERASE_SEQ_ERR + 8)
#define R2_ADDR_ERR (R1_ADDR_ERR + 8)
#define R2_PARAM_ERR (R1_PARAM_ERR + 8)
/* R3: size 5 bytes */
#define R3_OCR_MASK (0xffffffffUL)
#define R3_IDLE_STATE (R1_IDLE_STATE + 32)
#define R3_ERASE_RESET (R1_ERASE_RESET + 32)
#define R3_ILL_COMMAND (R1_ILL_COMMAND + 32)
#define R3_COM_CRC_ERR (R1_COM_CRC_ERR + 32)
#define R3_ERASE_SEQ_ERR (R1_ERASE_SEQ_ERR + 32)
#define R3_ADDR_ERR (R1_ADDR_ERR + 32)
#define R3_PARAM_ERR (R1_PARAM_ERR + 32)
/* Data Response: size 1 byte */
#define DR_STATUS_MASK 0x0e
#define DR_STATUS_ACCEPTED 0x05
#define DR_STATUS_CRC_ERR 0x0a
#define DR_STATUS_WRITE_ERR 0x0c

/* status bits for card types */
#define SD_RAW_SPEC_1 0
#define SD_RAW_SPEC_2 1
#define SD_RAW_SPEC_SDHC 2

#if !SD_RAW_SAVE_RAM
/* static data buffer for acceleration */
static uint8_t raw_block[512];
/* offset where the data within raw_block lies on the card */
static offset_t raw_block_address;
#endif

/* card type state */
static uint8_t sd_raw_card_type;

/* private helper functions */
static void sd_raw_send_byte(uint8_t b);
static uint8_t sd_raw_rec_byte(void);
static uint8_t sd_raw_send_command(uint8_t command, uint32_t arg);

uint8_t sd_raw_init(void) {
    /* enable outputs for MOSI, SCK, SS, input for MISO */
    DDRB |= _BV(DDB0);
    configure_pin_mosi();
    configure_pin_sck();
    configure_pin_ss();
    configure_pin_miso();

    unselect_card();

    /* initialize SPI with lowest frequency; max. 400kHz during identification mode of card */
    SPCR = _BV(MSTR) | _BV(SPE) | _BV(SPR1) | _BV(SPR0);
    /* SPCR = (0 << SPIE) | /1* SPI Interrupt Enable *1/ */
    /*        (1 << SPE)  | /1* SPI Enable *1/ */
    /*        (0 << DORD) | /1* Data Order: MSB first *1/ */
    /*        (1 << MSTR) | /1* Master mode *1/ */
    /*        (0 << CPOL) | /1* Clock Polarity: SCK low when idle *1/ */
    /*        (0 << CPHA) | /1* Clock Phase: sample on rising SCK edge *1/ */
    /*        (0 << SPR1) | /1* Clock Frequency: f_OSC / 128 *1/ */
    /*        (0 << SPR0); */
    SPSR = 0;
    /* SPSR = _BV(SPI2X); /1* doubled clock frequency *1/ */

    /* initialization procedure */
    sd_raw_card_type = 0;
    
    /* card needs 74 cycles minimum to start up */
    for (uint8_t i = 0; i < 10; ++i) {
        /* wait 8 clock cycles */
        sd_raw_rec_byte();
    }

    /* address card */
    select_card();

    /* reset card */
    uint8_t response;
    for (uint16_t i = 0; ; ++i) {
        response = sd_raw_send_command(CMD_GO_IDLE_STATE, 0);
        if (response == (1 << R1_IDLE_STATE)) {
            break;
        }

        if (i == 0x1ff) {
            unselect_card();
            xprintf("too many tries\n");
            return 0;
        }
    }

    /* check for version of SD card specification */
    response = sd_raw_send_command(CMD_SEND_IF_COND, 0x100 /* 2.7V - 3.6V */ | 0xaa /* test pattern */);
    if ((response & (1 << R1_ILL_COMMAND)) == 0) {
        sd_raw_rec_byte();
        sd_raw_rec_byte();
        if ((sd_raw_rec_byte() & 0x01) == 0) {
            xprintf("Voltage\n");
            return 0; /* card operation voltage range doesn't match */
        }
        uint8_t byte = sd_raw_rec_byte();
        if (byte != 0xaa) {
            xprintf("Wrong pattern: %X\n", byte);
            return 0; /* wrong test pattern */
        }

        /* card conforms to SD 2 card specification */
        sd_raw_card_type |= (1 << SD_RAW_SPEC_2);
    } else {
        /* determine SD/MMC card type */
        sd_raw_send_command(CMD_APP, 0);
        response = sd_raw_send_command(CMD_SD_SEND_OP_COND, 0);
        if ((response & (1 << R1_ILL_COMMAND)) == 0) {
            /* card conforms to SD 1 card specification */
            sd_raw_card_type |= (1 << SD_RAW_SPEC_1);
        } else {
            /* MMC card */
        }
    }

    /* wait for card to get ready */
    for (uint16_t i = 0; ; ++i) {
        if (sd_raw_card_type & ((1 << SD_RAW_SPEC_1) | (1 << SD_RAW_SPEC_2))) {
            uint32_t arg = 0;
            if(sd_raw_card_type & (1 << SD_RAW_SPEC_2))
                arg = 0x40000000;
            sd_raw_send_command(CMD_APP, 0);
            response = sd_raw_send_command(CMD_SD_SEND_OP_COND, arg);
        } else {
            response = sd_raw_send_command(CMD_SEND_OP_COND, 0);
        }

        if ((response & (1 << R1_IDLE_STATE)) == 0) {
            break;
        }

        if (i == 0x7f) {
            unselect_card();
            xprintf("Too many tries\n");
            return 0;
        }
    }

    if (sd_raw_card_type & (1 << SD_RAW_SPEC_2)) {
        if (sd_raw_send_command(CMD_READ_OCR, 0)) {
            unselect_card();
            xprintf("Read OCR\n");
            return 0;
        }

        if (sd_raw_rec_byte() & 0x40) {
            sd_raw_card_type |= (1 << SD_RAW_SPEC_SDHC);
        }

        sd_raw_rec_byte();
        sd_raw_rec_byte();
        sd_raw_rec_byte();
    }

    /* set block size to 512 bytes */
    if (sd_raw_send_command(CMD_SET_BLOCKLEN, 512)) {
        unselect_card();
        xprintf("Can't set block size\n");
        return 0;
    }

    /* deaddress card */
    unselect_card();

    /* switch to highest SPI frequency possible */
    SPCR &= ~((1 << SPR1) | (1 << SPR0)); /* Clock Frequency: f_OSC / 4 */
    SPSR |= (1 << SPI2X); /* Doubled Clock Frequency: f_OSC / 2 */

#if !SD_RAW_SAVE_RAM
    /* the first block is likely to be accessed first, so precache it here */
    raw_block_address = (offset_t) -1;
    if (!sd_raw_read(0, raw_block, sizeof(raw_block))) {
        xprintf("Can't read\n");
        return 0;
    }
#endif

    return 1;
}

void sd_raw_send_byte(uint8_t b) {
    SPDR = b;
    while (!(SPSR & (1 << SPIF)));
}

uint8_t sd_raw_rec_byte(void) {
    SPDR = 0xff;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}

uint8_t sd_raw_send_command(uint8_t command, uint32_t arg) {
    uint8_t response;

    /* wait some clock cycles */
    sd_raw_rec_byte();

    /* send command via SPI */
    sd_raw_send_byte(0x40 | command);
    sd_raw_send_byte((arg >> 24) & 0xff);
    sd_raw_send_byte((arg >> 16) & 0xff);
    sd_raw_send_byte((arg >> 8) & 0xff);
    sd_raw_send_byte(arg & 0xff);
    switch(command) {
        case CMD_GO_IDLE_STATE: sd_raw_send_byte(0x95); break;
        case CMD_SEND_IF_COND:  sd_raw_send_byte(0x87); break;
        default:                sd_raw_send_byte(0xff); break;
    }
    
    /* receive response */
    for (uint8_t i = 0; i < 10; ++i) {
        response = sd_raw_rec_byte();
        if (!(response & 0x80)) {
            break;
        }
    }

    return response;
}

uint8_t sd_raw_read(offset_t offset, uint8_t* buffer, uintptr_t length) {
    offset_t block_address;
    uint16_t block_offset;
    uint16_t read_length;
    while (length > 0) {
        /* determine byte count to read at once */
        block_offset = offset & 0x01ff;
        block_address = offset - block_offset;
        read_length = 512 - block_offset; /* read up to block border */
        if (read_length > length) {
            read_length = length;
        }
        
#if !SD_RAW_SAVE_RAM
        /* check if the requested data is cached */
        if (block_address != raw_block_address)
#endif
        {

            /* address card */
            select_card();

            /* send single block request */
            if (sd_raw_send_command(CMD_READ_SINGLE_BLOCK, (sd_raw_card_type & (1 << SD_RAW_SPEC_SDHC) ? block_address / 512 : block_address))) {
                unselect_card();
                return 0;
            }

            /* wait for data block (start byte 0xfe) */
            while (sd_raw_rec_byte() != 0xfe);

#if SD_RAW_SAVE_RAM
            /* read byte block */
            uint16_t read_to = block_offset + read_length;
            for (uint16_t i = 0; i < 512; ++i) {
                uint8_t b = sd_raw_rec_byte();
                if(i >= block_offset && i < read_to)
                    *buffer++ = b;
            }
#else
            /* read byte block */
            uint8_t* cache = raw_block;
            for (uint16_t i = 0; i < 512; ++i) {
                *cache++ = sd_raw_rec_byte();
            }
            raw_block_address = block_address;

            memcpy(buffer, raw_block + block_offset, read_length);
            buffer += read_length;
#endif
            
            /* read crc16 */
            sd_raw_rec_byte();
            sd_raw_rec_byte();
            
            /* deaddress card */
            unselect_card();

            /* let card some time to finish */
            sd_raw_rec_byte();
        }
#if !SD_RAW_SAVE_RAM
        else
        {
            /* use cached data */
            memcpy(buffer, raw_block + block_offset, read_length);
            buffer += read_length;
        }
#endif

        length -= read_length;
        offset += read_length;
    }
    return 1;
}

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (void) {
    if (!sd_raw_init()) {
        return STA_NOINIT;
    }
	return 0;
}

/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	BYTE* buff,		/* Pointer to the destination object */
	DWORD sector,	/* Sector number (LBA) */
	UINT offset,	/* Offset in the sector */
	UINT count		/* Byte count (bit15:destination) */
) {
    offset_t off = (offset_t) sector * 512 + offset;
    if (!sd_raw_read(off, buff, count)) {
        return RES_ERROR;
    }
    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/

DRESULT disk_writep (
	const BYTE* buff,		/* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
	DWORD sc		/* Sector number (LBA) or Number of bytes to send */
) {
	DRESULT res;

    res = RES_ERROR;

	if (!buff) {
		if (sc) {
			// Initiate write process
		} else {
			// Finalize write process
		}
	} else {
		// Send data to the disk
	}

	return res;
}
