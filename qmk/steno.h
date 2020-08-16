#ifndef _STENO_H_
#define _STENO_H_

#include "quantum.h"
#include <stdint.h>
#include "stroke.h"

#include "config.h"

#ifdef CONSOLE_ENABLE
#if defined(STENO_DEBUG) && defined(DEBUG_FLASH)
#define STENO_DEBUG_FLASH
#endif

#if defined(STENO_DEBUG) && defined(DEBUG_HIST)
#define STENO_DEBUG_HIST
#endif

#if defined(STENO_DEBUG) && defined(DEBUG_STROKE)
#define STENO_DEBUG_STROKE
#endif
#endif

#ifdef __AVR__
#define nrf_log_push(s) (s)

#define steno_error(format, ...) xprintf(format, ##__VA_ARGS__)
#define steno_error_ln(format, ...) xprintf(format "\n", ##__VA_ARGS__)
#if defined(CONSOLE_ENABLE) && defined(STENO_DEBUG)
#define steno_debug(format, ...) xprintf(format, ##__VA_ARGS__)
#define steno_debug_ln(format, ...) xprintf(format "\n", ##__VA_ARGS__)
#else
#define steno_debug(...)
#define steno_debug_ln(...)
#endif
#else

#define LED1 47
#define LED2 42
#define BUTTON 34
#define NEO_PIXEL 16

#if defined(CONSOLE_ENABLE) && defined(STENO_DEBUG)
#define steno_debug(format, ...) NRF_LOG_INFO(format, ##__VA_ARGS__)
#define steno_debug_ln(format, ...) NRF_LOG_INFO(format, ##__VA_ARGS__)
#define steno_error(format, ...) NRF_LOG_WARNING(format, ##__VA_ARGS__)
#define steno_error_ln(format, ...) NRF_LOG_WARNING(format, ##__VA_ARGS__)
#else
#define steno_debug(...)
#define steno_debug_ln(...)
#define steno_error(format, ...)
#define steno_error_ln(format, ...)
#endif
void tap_code(uint8_t code);
void tap_code16(uint16_t code);
#endif

typedef enum {
    BT_IDLE,
    BT_ACTIVE,
} bt_state_t;

typedef enum {
    DISP_NORMAL,
    DISP_STATUS,
} disp_state_t;

#define PACKET_SIZE 64
#define PAYLOAD_SIZE (PACKET_SIZE - 8)
#define MSG_SIZE (PAYLOAD_SIZE + 8 - 2)
#define MASS_WRITE_PACKET_NUM 28

typedef struct {
    uint8_t crc;
    uint8_t len;
} mass_write_info_t;

#endif
