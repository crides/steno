#include <avr/io.h>
#include "spi.h"

void spi_init(void) {
    /* DDRB |= _BV(DDB0); */
    DDRB |= _BV(DDB1);  // SCK
    DDRB |= _BV(DDB2);  // MOSI
    DDRB &= ~_BV(DDB3); // MISO
    DDRD |= _BV(DDD5);  // CS

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
