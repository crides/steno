#ifndef _STENO_H_
#define _STENO_H_

#include "quantum.h"
#include <stdint.h>
#include "stroke.h"

#include "config.h"

#define steno_error(format, ...) xprintf(format, ##__VA_ARGS__)
#define steno_error_ln(format, ...) xprintf(format "\n", ##__VA_ARGS__)
#if STENO_DEBUG
#define steno_debug(format, ...) xprintf(format, ##__VA_ARGS__)
#define steno_debug_ln(format, ...) xprintf(format "\n", ##__VA_ARGS__)
#else
#define steno_debug(...)
#define steno_debug_ln(...)
#endif

#define STENO_R_R 0x008100
#define STENO_STAR 0x1000

extern uint8_t stroke_start_ind;

void ebd_steno_init(void);
void ebd_steno_process_stroke(const uint32_t stroke);

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
