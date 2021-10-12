#ifndef _STENO_H_
#define _STENO_H_

#ifdef QMK_KEYBOARD
#include "quantum.h"
#include "config.h"
#endif
#include <stdint.h>
#include "stroke.h"

// printf formatter for double-word (32-bits)
#ifdef __AVR__
#define DWF(s) "%" s "lX"
#else
#define DWF(s) "%" s "X"
#endif

#ifdef QMK_KEYBOARD

#define steno_error(format, ...) xprintf(format, ##__VA_ARGS__)
#define steno_error_ln(format, ...) xprintf(format "\n", ##__VA_ARGS__)
#if STENO_DEBUG
#define steno_debug(format, ...) xprintf(format, ##__VA_ARGS__)
#define steno_debug_ln(format, ...) xprintf(format "\n", ##__VA_ARGS__)
#endif
#define log_strdup(s) (s)

#elif defined(CONFIG_ZMK_KEYBOARD_NAME)

#ifndef _STENO_C_
#include <logging/log.h>
LOG_MODULE_DECLARE(steno, CONFIG_ZMK_EMBEDDED_STENO_LOG_LEVEL);
#endif

#define steno_error(format, ...) LOG_ERR(format, ##__VA_ARGS__)
#define steno_error_ln(format, ...) LOG_ERR(format, ##__VA_ARGS__)
#if STENO_DEBUG
#define steno_debug(format, ...) LOG_DBG(format, ##__VA_ARGS__)
#define steno_debug_ln(format, ...) LOG_DBG(format, ##__VA_ARGS__)
#endif

#endif

#ifndef STENO_DEBUG
#define steno_debug(...)
#define steno_debug_ln(...)
#endif

#define STENO_R_R 0x008100
#define STENO_STAR 0x1000

#define TO_STR1(s) #s
#define TO_STR(s) TO_STR1(s)
#define BAT_INIT_MSG "Bat steno\ngit hash: " TO_STR(BAT_GIT_HASH)

extern uint8_t stroke_start_ind;

void ebd_steno_init(void);
void ebd_steno_process_stroke(const uint32_t stroke);

#ifdef CONFIG_ZMK_KEYBOARD_NAME
void steno_macro_init(void);
#endif

#define sizeof_array(a) (sizeof(a) / sizeof(a[0]))

#ifdef QMK_KEYBOARD
enum {
    STN__Z = SAFE_RANGE,
    STN__D,
    STN__S,
    STN__T,
    STN__G,
    STN__L,
    STN__B,
    STN__P,
    STN__R,
    STN__F,
    STN_U,
    STN_E,
    STN_STAR,
    STN_O,
    STN_A,
    STN_R_,
    STN_H_,
    STN_W_,
    STN_P_,
    STN_K_,
    STN_T_,
    STN_S_,
    STN_NUM,
};
#endif

#endif
