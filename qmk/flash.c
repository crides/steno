#include <string.h>
#include "flash.h"
#include "steno.h"

void flash_init(void) {
    /* enable outputs for MOSI, SCK, SS, input for MISO */
    DDRB |= _BV(DDB0);
    configure_pin_mosi();
    configure_pin_sck();
    configure_pin_ss();
    configure_pin_miso();

    unselect_card();

    SPCR = _BV(MSTR) | _BV(SPE);
    SPSR = _BV(SPI2X);
}

void spi_send_byte(uint8_t b) {
    SPDR = b;
    while(!(SPSR & _BV(SPIF)));
}

uint8_t spi_recv_byte(void) {
    SPDR = 0xff;
    while(!(SPSR & _BV(SPIF)));

    return SPDR;
}

void flash_read(uint32_t addr, uint8_t *buf, uint8_t len) {
    select_card();
    spi_send_byte(0x03);    // read 
    spi_send_byte((addr >> 16) & 0xFF);
    spi_send_byte((addr >> 8) & 0xFF);
    spi_send_byte(addr & 0xFF);
    for (uint8_t i = 0; i < len; i ++) {
        buf[i] = spi_recv_byte();
    }
    unselect_card();
}

void flash_write(uint32_t addr, uint8_t *buf, uint8_t len) {
    select_card();
    spi_send_byte(0x06);    // write enable
    unselect_card();

    select_card();
    spi_send_byte(0x02);    // program
    spi_send_byte((addr >> 16) & 0xFF);
    spi_send_byte((addr >> 8) & 0xFF);
    spi_send_byte(addr & 0xFF);
    for (uint8_t i = 0; i < len; i ++) {
        spi_send_byte(buf[i]);
    }
    unselect_card();

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

void flash_erase_page(uint32_t addr) {
    select_card();
    spi_send_byte(0x06);    // write enable
    unselect_card();

    select_card();
    spi_send_byte(0xD8);
    spi_send_byte((addr >> 16) & 0xFF);
    spi_send_byte((addr >> 8) & 0xFF);
    spi_send_byte(addr & 0xFF);
    unselect_card();

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
