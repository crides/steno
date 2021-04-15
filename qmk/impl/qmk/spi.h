#ifndef _SPI_H_
#define _SPI_H_

#include <avr/io.h>

#define configure_pin_sck() DDRB |= _BV(DDB1)
#define configure_pin_mosi() DDRB |= _BV(DDB2)
#define configure_pin_miso() DDRB &= ~_BV(DDB3)
#define configure_pin_ss() DDRD |= _BV(DDD5)

#define select_card() PORTD &= ~_BV(PORTD5)
#define unselect_card() PORTD |= _BV(PORTD5)

void spi_init(void);
void spi_send_byte(uint8_t);
void spi_send_word(uint16_t);
void spi_send_addr(uint32_t);
uint8_t spi_recv_byte(void);

#endif
