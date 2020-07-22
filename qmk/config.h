#ifndef CONFIG_H
#define CONFIG_H

#ifdef __AVR__
#include "config_common.h"
#endif

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

#define MATRIX_ROWS 3
#define MATRIX_COLS 10
#define THIS_DEVICE_ROWS 3
#define THIS_DEVICE_COLS 10
#define IS_LEFT_HAND true
#define ENABLE_STARTUP_ADV_NOLIST

#define BLE_NUS_MIN_INTERVAL 30 // default 60
#define BLE_NUS_MAX_INTERVAL 60 // default 75
#define BLE_HID_MAX_INTERVAL 30 // default 90
#define BLE_HID_SLAVE_LATENCY 99 // default 4

#define OLED_TIMEOUT 5000

#ifdef __AVR__
#define MATRIX_ROW_PINS { F7, F6, F5 }
#define MATRIX_COL_PINS { F4, F1, F0, D2, D3, D7, B6, B7, D6, C7 }
#else
#define MATRIX_ROW_PINS { 4, 5, 30 }
#define MATRIX_COL_PINS { 28, 2, 3, 24, 25, 7, 27, 6, 8, 41 }

#define CONFIG_PIN_SCL 11
#define CONFIG_PIN_SDA 12

#define USE_BATTERY_PIN NRF_SAADC_INPUT_AIN5
#endif

#define DIODE_DIRECTION COL2ROW
#define DEBOUNCING_DELAY 5
#define LOCKING_SUPPORT_ENABLE
#define LOCKING_RESYNC_ENABLE
#define PREVENT_STUCK_MODIFIERS

#define NO_ACTION_LAYER
#define NO_ACTION_TAPPING
#define NO_ACTION_ONESHOT
#define NO_ACTION_MACRO
#define NO_ACTION_FUNCTION

#endif
