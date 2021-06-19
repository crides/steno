// Functions for interacting with SPI flash
#include "store.h"
#include "steno.h"
#include <device.h>
#include <drivers/flash.h>

#define FLASH_ERASE_PAGE_SIZE 0x1000    // 4KiB

#ifdef STENO_DEBUG_FLASH
uint8_t flash_debug_enable = 0;
#endif

#define FLASH_DEVICE DT_LABEL(DT_INST(0, jedec_spi_nor))

static const struct device *flash_dev;

void store_init(void) {
    flash_dev = device_get_binding(FLASH_DEVICE);
	if (!flash_dev) {
		LOG_ERR("SPI flash driver %s was not found!\n", FLASH_DEVICE);
	}
}

void store_read(uint32_t const offset, uint8_t *const buf, const uint8_t len) {
#ifdef STENO_DEBUG_FLASH
    if (flash_debug_enable) {
        steno_debug_ln("flash_read(# 0x%02x @ 0x%06x)", len, addr);
    }
#endif
    flash_read(flash_dev, offset, buf, len);
}

void store_flush(void) {
}

void store_write_direct(const uint32_t offset, const uint8_t *const buf, const uint8_t len) {
    flash_write(flash_dev, offset, buf, len);
}

void store_erase_partial(const uint32_t offset, const uint8_t len) {
    const uint32_t page_aligned = offset & 0xFFF000;    // -1 * FLASH_ERASE_PAGE_SIZE ?
    uint8_t page_buf[FLASH_ERASE_PAGE_SIZE];
    flash_read(flash_dev, page_aligned, page_buf, FLASH_ERASE_PAGE_SIZE);
    memset(&page_buf[offset - page_aligned], 0xFF, len);
    flash_erase(flash_dev, page_aligned, FLASH_ERASE_PAGE_SIZE);
    flash_write(flash_dev, page_aligned, page_buf, FLASH_ERASE_PAGE_SIZE);
}
