#ifndef _STENO_H_
#define _STENO_H_

#include "quantum.h"
#include <stdint.h>
#include "stroke.h"

#define CUSTOM_STENO
#define USE_SPI_FLASH
/* #define STENO_DEBUG */

#ifdef STENO_DEBUG
#define steno_debug(format, ...) xprintf(format, ##__VA_ARGS__)
#else
#define steno_debug(...)
#endif

#define PACKET_SIZE 64
#define PAYLOAD_SIZE (PACKET_SIZE - 8)
#define MSG_SIZE (PAYLOAD_SIZE + 8 - 2)
#define MASS_WRITE_PACKET_NUM 28

typedef struct {
    uint8_t crc;
    uint8_t len;
} mass_write_info_t;

#endif
