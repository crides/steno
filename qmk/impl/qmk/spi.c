#include "spi.h"

void spi_init(void) {
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

void spi_send_word(uint16_t w) {
    spi_send_byte(w >> 8);
    spi_send_byte(w);
}

void spi_send_addr(uint32_t addr) {
    spi_send_byte((addr >> 16) & 0xFF);
    spi_send_byte((addr >> 8) & 0xFF);
    spi_send_byte(addr & 0xFF);
}

uint8_t spi_recv_byte(void) {
    SPDR = 0xff;
    while(!(SPSR & _BV(SPIF)));

    return SPDR;
}
