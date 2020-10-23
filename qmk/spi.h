#ifndef _SPI_H_
#define _SPI_H_

#include <avr/io.h>

#define configure_pin_sck() DDRB |= _BV(DDB1)
#define configure_pin_mosi() DDRB |= _BV(DDB2)
#define configure_pin_miso() DDRB &= ~_BV(DDB3)
#define configure_pin_ss() DDRC |= _BV(DDC6)
#define configure_lcd_cs() DDRF |= _BV(DDF6)

#define select_card() PORTC &= ~_BV(PORTC6)
#define unselect_card() PORTC |= _BV(PORTC6)

#define select_lcd() PORTF &= ~_BV(PORTF6)
#define unselect_lcd() PORTF |= _BV(PORTF6)

void spi_init(void);
void spi_send_byte(uint8_t);
void spi_send_word(uint16_t);
void spi_send_addr(uint32_t);
uint8_t spi_recv_byte(void);

#endif
