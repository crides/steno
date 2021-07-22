// Functions for interacting with SPI flash
#include "store.h"
#include "steno.h"
#include <device.h>
#include <drivers/flash.h>

#define FLASH_ERASE_PAGE_SIZE 0x1000    // 4KiB

#ifdef STENO_DEBUG_FLASH
uint8_t flash_debug_enable = 0;
#endif

#define FLASH_DEVICE DT_LABEL(DT_CHOSEN(zmk_steno_flash))

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
    const int ret = flash_read(flash_dev, offset, buf, len);
    if (ret != 0) {
        steno_debug_ln("read %u", ret);
    }
}

void store_flush(void) {
}

void store_write_direct(const uint32_t offset, const uint8_t *const buf, const uint8_t len) {
    const int ret = flash_write(flash_dev, offset, buf, len);
    if (ret != 0) {
        steno_debug_ln("write %u", ret);
    }
}

void store_erase_partial(const uint32_t offset, const uint8_t len) {
    const uint32_t page_aligned = offset & 0xFFF000;    // -1 * FLASH_ERASE_PAGE_SIZE ?
    uint8_t page_buf[FLASH_ERASE_PAGE_SIZE];
    int ret;
    ret = flash_read(flash_dev, page_aligned, page_buf, FLASH_ERASE_PAGE_SIZE);
    if (ret != 0) {
        steno_debug_ln("read %u", ret);
    }
    memset(&page_buf[offset - page_aligned], 0xFF, len);
    ret = flash_erase(flash_dev, page_aligned, FLASH_ERASE_PAGE_SIZE);
    if (ret != 0) {
        steno_debug_ln("erase %u", ret);
    }
    ret = flash_write(flash_dev, page_aligned, page_buf, FLASH_ERASE_PAGE_SIZE);
    if (ret != 0) {
        steno_debug_ln("write %u", ret);
    }
}
