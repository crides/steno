#ifndef CONFIG_H
#define CONFIG_H

#include "config_common.h"

/* USB Device descriptor parameter */
#define VENDOR_ID       0xFEED
#define PRODUCT_ID      0x6060
#define DEVICE_VER      0x0001
#define MANUFACTURER    Crides
#define PRODUCT         Steno
#define DESCRIPTION     28 key steno board

#define MATRIX_ROWS 3
#define MATRIX_COLS 11

#define MATRIX_ROW_PINS { F7, F6, F5 }
#define MATRIX_COL_PINS { B7, B6, D6, D7, C6, D3, D2, D0, F0, F1, F4 }

#define DIODE_DIRECTION COL2ROW
#define DEBOUNCING_DELAY 5
#define LOCKING_SUPPORT_ENABLE
#define LOCKING_RESYNC_ENABLE
#define PREVENT_STUCK_MODIFIERS
#define FORCE_NKRO

#endif
