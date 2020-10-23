#pragma once

#ifdef __AVR__
#include <avr/io.h>

#define configure_pin_sck() DDRB |= _BV(DDB1)
#define configure_pin_mosi() DDRB |= _BV(DDB2)
#define configure_pin_miso() DDRB &= ~_BV(DDB3)
#define configure_pin_ss() DDRC |= _BV(DDC6)

#define select_card() PORTC &= ~_BV(PORTC6)
#define unselect_card() PORTC |= _BV(PORTC6)
#else
#include <stdint.h>
#endif

void flash_init(void);
void flash_read(uint32_t addr, uint8_t *buf, uint8_t len);
void flash_write(uint32_t addr, uint8_t *buf, uint8_t len);
void flash_erase_4k(uint32_t addr);
void flash_erase_64k(uint32_t addr);
void flash_erase_device(void);
