// Functions for interacting with SPI flash
#include <string.h>
#include "flash.h"
#include "steno.h"
#include "spi.h"

#ifdef STENO_DEBUG_FLASH
uint8_t flash_debug_enable = 0;
#endif

void flash_init(void) {
    // Assume SPI inited
}

void flash_read(uint32_t addr, uint8_t *buf, uint8_t len) {
#ifdef STENO_DEBUG_FLASH
    if (flash_debug_enable) {
        steno_debug_ln("flash_read(# 0x%02X @ 0x%06lX)", len, addr);
    }
#endif
    select_card();
    spi_send_byte(0x03);    // read 
    spi_send_addr(addr);
    for (uint8_t i = 0; i < len; i ++) {
        buf[i] = spi_recv_byte();
    }
    unselect_card();
}

// Read a program page into buffer
void flash_read_page(uint32_t addr, uint8_t *buf) {
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

void flash_flush(void) {
    select_card();
    while (1) {
        spi_send_byte(0x05);    // read status reg
        uint8_t status = spi_recv_byte();
        if (!(status & 0x01)) {
            break;
        }
    }
    unselect_card();
}

void flash_prep_write(void) {
    flash_flush();
    select_card();
    spi_send_byte(0x06);    // write enable
    unselect_card();
}

void flash_write(uint32_t addr, uint8_t *buf, uint8_t len) {
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

void flash_write_page(uint32_t addr, uint8_t *buf) {
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

void flash_erase_64k(uint32_t addr) {
    flash_prep_write();
    select_card();
    spi_send_byte(0xD8);
    spi_send_addr(addr);
    unselect_card();
}

void flash_erase_4k(uint32_t addr) {
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

void flash_erase_device(void) {
    flash_prep_write();
    select_card();
    spi_send_byte(0xC7);
    unselect_card();
}

uint64_t flash_check_crc_range(uint32_t start, uint32_t end, uint64_t crc) {
    select_card();
    spi_send_byte(0x9B);
    spi_send_byte(0x27);
    spi_send_byte(0xFE);
    for (uint8_t i = 0; i < 8; i ++) {
        spi_send_byte((crc >> (i * 8)) & 0xFF);
    }
    spi_send_byte(start & 0xFF);
    spi_send_byte((start >> 8) & 0xFF);
    spi_send_byte((start >> 16) & 0xFF);
    spi_send_byte((start >> 24) & 0xFF);
    spi_send_byte(end & 0xFF);
    spi_send_byte((end >> 8) & 0xFF);
    spi_send_byte((end >> 16) & 0xFF);
    spi_send_byte((end >> 24) & 0xFF);
    unselect_card();
    _delay_ms(((end - start) >> 20) * 3);    // Assuming 3ms per 1MB block; typ 1.3ms
    select_card();
    spi_send_byte(0x70);        // Flag status register
    uint8_t flag = spi_recv_byte();
    unselect_card();
    if (flag & 0x10) {
        steno_error_ln("CRC check error");
        select_card();
        spi_send_byte(0x96);
        for (uint8_t i = 0; i < 8; i ++) {
            spi_recv_byte();
        }
        uint64_t actual_crc;
        for (uint8_t i = 0; i < 8; i ++) {
            actual_crc |= (uint64_t) spi_recv_byte() << (i * 8);
        }
        return actual_crc;
    } else {
        steno_debug_ln("CRC check OK");
        return 0;
    }
}
