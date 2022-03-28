// Functions for interacting with SPI flash
#include <string.h>
#include "store.h"
#include "steno.h"
#include "spi.h"

#ifdef STENO_DEBUG_FLASH
uint8_t flash_debug_enable = 0;
#endif

// Program page size
#define FLASH_PP_SIZE 256
#define FLASH_ERASED_BYTE 0xFF

void store_init(void) {
    spi_init();
}

void store_read(const uint32_t offset, uint8_t *const buf, const uint8_t len) {
#ifdef STENO_DEBUG_FLASH
    if (flash_debug_enable) {
        steno_debug_ln("flash_read(# 0x%02X @ 0x%06lX)", len, offset);
    }
#endif
    select_card();
    spi_send_byte(0x03);    // read 
    spi_send_addr(offset);
    for (uint8_t i = 0; i < len; i ++) {
        buf[i] = spi_recv_byte();
    }
    unselect_card();
}

// Read a program page into buffer
static void flash_read_page(uint32_t addr, uint8_t *buf) {
#ifdef STENO_DEBUG_FLASH
    if (flash_debug_enable) {
        steno_debug_ln("flash_read_page(@ 0x%06lX)", addr);
    }
#endif
    select_card();
    spi_send_byte(0x03);    // read 
    spi_send_addr(addr);
    for (uint8_t i = 0; i < 255; i ++) {
        buf[i] = spi_recv_byte();
    }
    buf[255] = spi_recv_byte();
    unselect_card();
}

static void flash_flush(void) {
    select_card();
    while (1) {
        spi_send_byte(0x05);    // read status reg
        const uint8_t status = spi_recv_byte();
        if (!(status & 0x01)) {
            break;
        }
    }
    unselect_card();
}

void store_flush(void) {
    flash_flush();
}

static void flash_prep_write(void) {
    flash_flush();
    select_card();
    spi_send_byte(0x06);    // write enable
    unselect_card();
}

static void flash_write(const uint32_t addr, const uint8_t *const buf, const uint8_t len) {
#ifdef STENO_DEBUG_FLASH
    if (flash_debug_enable) {
        steno_debug_ln("flash_write(# 0x%02X @ 0x%06lX)", len, addr);
    }
#endif
    flash_prep_write();
    select_card();
    spi_send_byte(0x02);    // program
    spi_send_addr(addr);
    for (uint8_t i = 0; i < len; i ++) {
        spi_send_byte(buf[i]);
    }
    unselect_card();
}

void store_write_direct(const uint32_t offset, const uint8_t *const buf, const uint8_t len) {
    flash_write(offset, buf, len);
}

static void flash_write_page(const uint32_t addr, const uint8_t *const buf) {
#ifdef STENO_DEBUG_FLASH
    if (flash_debug_enable) {
        steno_debug_ln("flash_write_page(@ 0x%06lX)", addr);
    }
#endif
    flash_prep_write();
    select_card();
    spi_send_byte(0x02);    // program
    spi_send_addr(addr);
    for (uint8_t i = 0; i < 255; i ++) {
        spi_send_byte(buf[i]);
    }
    spi_send_byte(buf[255]);
    unselect_card();
}

// HACK uses private/platform specific interface
#ifndef STENO_FLASH_LOGGING
static
#endif
void flash_erase_4k(const uint32_t addr) {
#ifdef STENO_DEBUG_FLASH
    if (flash_debug_enable) {
        steno_debug_ln("flash_erase_4k(@ 0x%06lX)", addr);
    }
#endif
    flash_prep_write();
    select_card();
    spi_send_byte(0x20);
    spi_send_addr(addr);
    unselect_card();
}

void store_erase_partial(const uint32_t offset, const uint8_t len) {
    uint8_t page_buffer[FLASH_PP_SIZE];
    const uint32_t block_addr = offset & 0xFFF000; // Alighed to 4k, smallest Erase Unit
    flash_erase_4k(SCRATCH_START);

    const uint32_t page_addr = offset & 0xFFFF00; // Aligned to 256 (PP_SIZE)
    for (uint32_t addr = block_addr, scratch_addr = SCRATCH_START; addr < block_addr + 0x1000; addr += FLASH_PP_SIZE) {
        flash_flush();
        flash_read_page(addr, page_buffer);
        if (page_addr == addr) {
            const uint8_t page_offset = offset & 0xFF;
            memset(page_buffer + page_offset, FLASH_ERASED_BYTE, len);
#ifdef STENO_DEBUG_DICTED
            steno_debug_ln("cleared: offset: %02X, len: %02X", page_offset, len);
#endif
        }
        flash_write_page(scratch_addr, page_buffer);
        scratch_addr += FLASH_PP_SIZE;
    }

    flash_erase_4k(block_addr);
    for (uint32_t addr = block_addr, scratch_addr = SCRATCH_START; addr < block_addr + 0x1000; addr += FLASH_PP_SIZE) {
        flash_flush();
        flash_read_page(scratch_addr, page_buffer);
        flash_write_page(addr, page_buffer);
        scratch_addr += FLASH_PP_SIZE;
    }
    flash_flush();
}

void store_rewrite_start(void) {
    flash_prep_write();
    select_card();
    // Device erase
    spi_send_byte(0xC7);
    unselect_card();
}

void store_rewrite_write(const uint32_t offset, const uint8_t *const buf) {
    flash_write_page(offset, buf);
}
