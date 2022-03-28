#include "steno.h"

#define KEYMAP( \
    L00 , L01 , L02 , L03 , L04 , L05 , R00    , R01 , R02 , R03 , R04 , R05 , \
    L10 , L11 , L12 , L13 , L14 , L15 , R10    , R11 , R12 , R13 , R14 , R15 , \
    L20 , L21 , L22 , R20 , R21 , R22) \
    { \
        {L00,     L01,     L02,     L03,     L04,     L05},     \
        {L10,     L11,     L12,     L13,     L14,     L15},     \
        {L20,     L21,     L22,     R22,     R21,     R20},     \
        {R05,     R04,     R03,     R02,     R01,     R00},     \
        {R15,     R14,     R13,     R12,     R11,     R10},     \
    }

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    KEYMAP(
		STN_S_,  STN_S_,  STN_T_,  STN_P_,  STN_H_,  STN_STAR,  STN_STAR,  STN__F,  STN__P,  STN__L,  STN__T,  STN__D,
		STN_S_,  STN_S_,  STN_K_,  STN_W_,  STN_R_,  STN_STAR,  STN_STAR,  STN__R,  STN__B,  STN__G,  STN__S,  STN__Z,
		STN_NUM,   STN_A,   STN_O,   STN_E,   STN_U,  STN_NUM),
};
