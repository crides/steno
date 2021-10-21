#ifndef _SPI_H_
#define _SPI_H_

#define __AVR_ATmega32U4__
#include <avr/io.h>

inline void select_card() {
    PORTD &= ~_BV(PORTD5);
}

inline void unselect_card() {
    PORTD |= _BV(PORTD5);
}

void spi_init(void);
void spi_send_byte(uint8_t);
void spi_send_word(uint16_t);
void spi_send_addr(uint32_t);
uint8_t spi_recv_byte(void);

#endif
