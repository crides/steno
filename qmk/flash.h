#pragma once

#ifdef __AVR__
#include <avr/io.h>

#else
#include <stdint.h>
#endif

void flash_init(void);
void flash_read(uint32_t addr, uint8_t *buf, uint8_t len);
void flash_write(uint32_t addr, uint8_t *buf, uint8_t len);
void flash_erase_4k(uint32_t addr);
void flash_erase_64k(uint32_t addr);
void flash_erase_device(void);
