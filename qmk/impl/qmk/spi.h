#ifndef _SPI_H_
#define _SPI_H_

#ifndef __AVR_ATmega32U4__
#define __AVR_ATmega32U4__
#endif
#include <avr/io.h>

static inline void select_card(void) {
    PORTD &= ~_BV(PORTD5);
}

static inline void unselect_card(void) {
    PORTD |= _BV(PORTD5);
}

void spi_init(void);
void spi_send_byte(uint8_t);
void spi_send_word(uint16_t);
void spi_send_addr(uint32_t);
uint8_t spi_recv_byte(void);

#endif
