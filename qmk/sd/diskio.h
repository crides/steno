/*-----------------------------------------------------------------------
/  PFF - Low level disk interface modlue include file    (C)ChaN, 2014
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#include "pff.h"

#include <stdint.h>

// SD card configuration
/**
 * \ingroup sd_raw_config
 * Controls MMC/SD write support.
 *
 * Set to 1 to enable MMC/SD write support, set to 0 to disable it.
 */
#define SD_RAW_WRITE_SUPPORT 0

/**
 * \ingroup sd_raw_config
 * Controls MMC/SD write buffering.
 *
 * Set to 1 to buffer write accesses, set to 0 to disable it.
 *
 * \note This option has no effect when SD_RAW_WRITE_SUPPORT is 0.
 */
#define SD_RAW_WRITE_BUFFERING 1

/**
 * \ingroup sd_raw_config
 * Controls MMC/SD access buffering.
 * 
 * Set to 1 to save static RAM, but be aware that you will
 * lose performance.
 *
 * \note When SD_RAW_WRITE_SUPPORT is 1, SD_RAW_SAVE_RAM will
 *       be reset to 0.
 */
#define SD_RAW_SAVE_RAM 0

/* defines for customisation of sd/mmc port access */
#define configure_pin_sck() DDRB |= (1 << DDB1)
#define configure_pin_mosi() DDRB |= (1 << DDB2)
#define configure_pin_miso() DDRB &= ~(1 << DDB3)
#define configure_pin_ss() DDRC |= (1 << DDC6)

#define select_card() PORTC &= ~(1 << PORTC6)
#define unselect_card() PORTC |= (1 << PORTC6)

typedef uint32_t offset_t;

/* configuration checks */
#if SD_RAW_WRITE_SUPPORT
#undef SD_RAW_SAVE_RAM
#define SD_RAW_SAVE_RAM 0
#else
#undef SD_RAW_WRITE_BUFFERING
#define SD_RAW_WRITE_BUFFERING 0
#endif
// SD configuration end

/**
 * The card's layout is harddisk-like, which means it contains
 * a master boot record with a partition table.
 */
#define SD_RAW_FORMAT_HARDDISK 0
/**
 * The card contains a single filesystem and no partition table.
 */
#define SD_RAW_FORMAT_SUPERFLOPPY 1
/**
 * The card's layout follows the Universal File Format.
 */
#define SD_RAW_FORMAT_UNIVERSAL 2
/**
 * The card's layout is unknown.
 */
#define SD_RAW_FORMAT_UNKNOWN 3

/**
 * This struct is used by sd_raw_get_info() to return
 * manufacturing and status information of the card.
 */
struct sd_raw_info
{
    /**
     * A manufacturer code globally assigned by the SD card organization.
     */
    uint8_t manufacturer;
    /**
     * A string describing the card's OEM or content, globally assigned by the SD card organization.
     */
    uint8_t oem[3];
    /**
     * A product name.
     */
    uint8_t product[6];
    /**
     * The card's revision, coded in packed BCD.
     *
     * For example, the revision value \c 0x32 means "3.2".
     */
    uint8_t revision;
    /**
     * A serial number assigned by the manufacturer.
     */
    uint32_t serial;
    /**
     * The year of manufacturing.
     *
     * A value of zero means year 2000.
     */
    uint8_t manufacturing_year;
    /**
     * The month of manufacturing.
     */
    uint8_t manufacturing_month;
    /**
     * The card's total capacity in bytes.
     */
    offset_t capacity;
    /**
     * Defines wether the card's content is original or copied.
     *
     * A value of \c 0 means original, \c 1 means copied.
     */
    uint8_t flag_copy;
    /**
     * Defines wether the card's content is write-protected.
     *
     * \note This is an internal flag and does not represent the
     *       state of the card's mechanical write-protect switch.
     */
    uint8_t flag_write_protect;
    /**
     * Defines wether the card's content is temporarily write-protected.
     *
     * \note This is an internal flag and does not represent the
     *       state of the card's mechanical write-protect switch.
     */
    uint8_t flag_write_protect_temp;
    /**
     * The card's data layout.
     *
     * See the \c SD_RAW_FORMAT_* constants for details.
     *
     * \note This value is not guaranteed to match reality.
     */
    uint8_t format;
};

uint8_t sd_raw_init(void);
uint8_t sd_raw_read(offset_t offset, uint8_t* buffer, uintptr_t length);

/* Status of Disk Functions */
typedef BYTE	DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Function succeeded */
	RES_ERROR,		/* 1: Disk error */
	RES_NOTRDY,		/* 2: Not ready */
	RES_PARERR		/* 3: Invalid parameter */
} DRESULT;

/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS disk_initialize (void);
DRESULT disk_readp (BYTE* buff, DWORD sector, UINT offser, UINT count);
DRESULT disk_writep (const BYTE* buff, DWORD sc);

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */

#ifdef __cplusplus
}
#endif

#endif	/* _DISKIO_DEFINED */
