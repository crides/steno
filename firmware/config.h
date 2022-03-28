#ifndef CONFIG_H
#define CONFIG_H

#include "config_common.h"

/* USB Device descriptor parameter */
#define VENDOR_ID       0xFEED
#define PRODUCT_ID      0x6061
#define DEVICE_VER      0x0001
#define MANUFACTURER    Crides
#define PRODUCT         Steno
#define SERIAL_NUMBER   v2
#define DESCRIPTION     28 key steno board

#define MATRIX_ROWS 5
#define MATRIX_COLS 6

#define OLED_TIMEOUT 3000
#define OLED_DISABLE_TIMEOUT

#define MATRIX_ROW_PINS { F0, F1, D3, D1, D2 }
#define MATRIX_COL_PINS { D7, B4, B5, B6, C6, C7 }

#define DIODE_DIRECTION ROW2COL
#define DEBOUNCING_DELAY 5
#define LOCKING_SUPPORT_ENABLE
#define LOCKING_RESYNC_ENABLE
#define PREVENT_STUCK_MODIFIERS
#define UNICODE_SELECTED_MODES UC_LNX

#ifdef STENO_DEBUG
#undef NO_PRINT
#endif

#define LAYER_STATE_8BIT
#define NO_RESET
#define NO_ACTION_LAYER
#define NO_ACTION_TAPPING
#define NO_ACTION_ONESHOT
#define NO_ACTION_MACRO
#define NO_ACTION_FUNCTION

#endif
