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
#define DESCRIPTION     30 key steno board

#define MATRIX_ROWS 3
#define MATRIX_COLS 10

#define OLED_TIMEOUT 3000
#define OLED_DISABLE_TIMEOUT

#define MATRIX_ROW_PINS { F7, F6, F5 }
#define MATRIX_COL_PINS { F4, F1, F0, D2, D3, D7, B6, B7, D6, C7 }

#define DIODE_DIRECTION COL2ROW
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
