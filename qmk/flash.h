#pragma once

#ifdef __AVR__
#include <avr/io.h>
#else
#include <stdint.h>
#endif

// Program page size
#define FLASH_PP_SIZE 256
#define FLASH_ERASED_BYTE 0xFF

void flash_init(void);
void flash_read(uint32_t addr, uint8_t *buf, uint8_t len);
void flash_read_page(uint32_t addr, uint8_t *buf);
void flash_write(uint32_t addr, uint8_t *buf, uint8_t len);
void flash_write_page(uint32_t addr, uint8_t *buf);
void flash_erase_4k(uint32_t addr);
void flash_erase_64k(uint32_t addr);
void flash_erase_device(void);
uint64_t flash_check_crc_range(uint32_t start, uint32_t end, uint64_t crc);
