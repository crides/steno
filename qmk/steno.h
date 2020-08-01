#ifndef _STENO_H_
#define _STENO_H_

#include "quantum.h"
#include <stdint.h>
#include "stroke.h"

/* #include "config.h" */

#define CUSTOM_STENO
#define USE_SPI_FLASH
#define STENO_PHONE
#define STENO_DEBUG_FLASH

#ifdef CONSOLE_ENABLE
#define STENO_DEBUG
#endif

#ifdef __AVR__
#define steno_error(format, ...) xprintf(format, ##__VA_ARGS__)
#define steno_error_ln(format, ...) xprintf(format "\n", ##__VA_ARGS__)
#ifdef STENO_DEBUG
#define steno_debug(format, ...) xprintf(format, ##__VA_ARGS__)
#define steno_debug_ln(format, ...) xprintf(format "\n", ##__VA_ARGS__)
#else
#define steno_debug(...)
#define steno_debug_ln(...)
#endif
#else
/* #define NRF_LOG_MODULE_NAME steno */
#define LED1 47
#define LED2 42
#define BUTTON 34
#define NEO_PIXEL 16

#define steno_error(format, ...) NRF_LOG_WARNING(format, ##__VA_ARGS__)
#define steno_error_ln(format, ...) NRF_LOG_WARNING(format, ##__VA_ARGS__)
#ifdef STENO_DEBUG
#define steno_debug(format, ...) NRF_LOG_INFO(format, ##__VA_ARGS__)
#define steno_debug_ln(format, ...) NRF_LOG_INFO(format, ##__VA_ARGS__)
#else
#define steno_debug(...)
#define steno_debug_ln(...)
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

#define STATUS_STAY_TIME 2000
#define BUTTON_HOLD_TIME 1000
#define BT_ACTIVE_HOLD_TIME 10000

#define PACKET_SIZE 64
#define PAYLOAD_SIZE (PACKET_SIZE - 8)
#define MSG_SIZE (PAYLOAD_SIZE + 8 - 2)
#define MASS_WRITE_PACKET_NUM 28

typedef struct {
    uint8_t crc;
    uint8_t len;
} mass_write_info_t;

#endif
