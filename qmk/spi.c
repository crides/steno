#include "spi.h"

void spi_init(void) {
    DDRB |= _BV(DDB0);
    configure_pin_mosi();
    configure_pin_sck();
    configure_pin_ss();
    configure_pin_miso();
    configure_lcd_cs();

    unselect_card();
    unselect_lcd();

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

uint8_t spi_recv_byte(void) {
    SPDR = 0xff;
    while(!(SPSR & _BV(SPIF)));

    return SPDR;
}
