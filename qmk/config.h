#ifndef CONFIG_H
#define CONFIG_H

#ifdef __AVR__
#include "config_common.h"
#endif

#define CUSTOM_STENO
#define USE_SPI_FLASH
/* #define STENO_PHONE */

#define STENO_DEBUG
#define DEBUG_FLASH
#define DEBUG_STROKE
#define DEBUG_HIST

#define STATUS_STAY_TIME 1500
#define BUTTON_HOLD_TIME 1000
#define BT_ACTIVE_HOLD_TIME 3000

#define MATRIX_SCAN_MS 20

/* USB Device descriptor parameter */
#define VENDOR_ID       0xFEED
#define PRODUCT_ID      0x6061
#define DEVICE_VER      0x0001
#define MANUFACTURER    Crides
#define PRODUCT         Steno
#define SERIAL_NUMBER   v2
#define DESCRIPTION     28 key steno board
#define RAW_USAGE_PAGE  0xFF60
#define RAW_USAGE_ID    0x61

#ifdef __AVR__
#ifdef CUSTOM_STENO
#undef VIRTSER_ENABLE
#endif
#endif

#ifdef STENO_PHONE
#define MATRIX_ROWS 5
#define MATRIX_COLS 6
#else
#define MATRIX_ROWS 5
#define MATRIX_COLS 6
#endif

#define THIS_DEVICE_ROWS MATRIX_ROWS
#define THIS_DEVICE_COLS MATRIX_COLS
#define IS_LEFT_HAND true
#define ENABLE_STARTUP_ADV_NOLIST

#define BLE_NUS_MIN_INTERVAL 30 // default 60
#define BLE_NUS_MAX_INTERVAL 40 // default 75
#define BLE_HID_MAX_INTERVAL 30 // default 90
#define BLE_HID_SLAVE_LATENCY 99 // default 4

#define OLED_TIMEOUT 3000
#define OLED_DISABLE_TIMEOUT

#ifdef __AVR__
#ifdef STENO_PHONE
#define MATRIX_ROW_PINS { C7, D6, D7, B7, B6 }
#define MATRIX_COL_PINS { F7, F6, F5, F4, F1, F0 }
#else
#define MATRIX_ROW_PINS { D1, D2, D3, F0, F1 }
#define MATRIX_COL_PINS { D7, B4, B5, B6, C6, C7 }
#endif
#else
#ifdef STENO_PHONE
#define MATRIX_ROW_PINS { 41, 8, 7, 6, 27 }
#define MATRIX_COL_PINS { 4, 5, 30, 28, 2, 3 }
#else
#define MATRIX_ROW_PINS { 4, 5, 30 }
#define MATRIX_COL_PINS { 28, 2, 3, 24, 25, 7, 27, 6, 8, 41 }
#endif

#define CONFIG_PIN_SCL 11
#define CONFIG_PIN_SDA 12

#define USE_BATTERY_PIN NRF_SAADC_INPUT_AIN5
#endif

#define DIODE_DIRECTION COL2ROW
#define DEBOUNCING_DELAY 5
#define LOCKING_SUPPORT_ENABLE
#define LOCKING_RESYNC_ENABLE
#define PREVENT_STUCK_MODIFIERS
#define UNICODE_SELECTED_MODES UC_LNX

#define NO_ACTION_LAYER
#define NO_ACTION_TAPPING
#define NO_ACTION_ONESHOT
#define NO_ACTION_MACRO
#define NO_ACTION_FUNCTION

#endif
