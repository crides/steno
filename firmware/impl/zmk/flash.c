// Functions for interacting with SPI flash
#include "store.h"
#include "steno.h"
#include <zephyr/drivers/flash.h>

#define FLASH_ERASE_PAGE_SIZE 0x1000    // 4KiB

#ifdef STENO_DEBUG_FLASH
uint8_t flash_debug_enable = 1;
#endif

// don't polute cache
#define RPI_FLASH_BASE 0x13000000

void store_init(void) { }

void store_read(uint32_t const offset, uint8_t *const buf, const uint8_t len) {
    const uint8_t *addr = (uint8_t *) (RPI_FLASH_BASE + offset);
    memcpy(buf, addr, len);
#ifdef STENO_DEBUG_FLASH
    if (flash_debug_enable) {
        steno_debug_ln("(#0x%02x @ 0x%06x) => %02x %02x %02x %02x", len, offset, buf[0], buf[1], buf[2], buf[3]);
    }
#endif
}

void store_flush(void) { }

void store_write_direct(const uint32_t offset, const uint8_t *const buf, const uint8_t len) { }

void store_erase_partial(const uint32_t offset, const uint8_t len) { }
