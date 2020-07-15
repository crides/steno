#include <string.h>
#include "steno.h"
#include "keymap_steno.h"
#include "action_layer.h"
#include "eeconfig.h"

#define KEYMAP( \
    L00 , L01 , L02 , L03 , L04 , R00    , R01 , R02 , R03 , R04 , R05 , \
    L10 , L11 , L12 , L13 , L14 , R10    , R11 , R12 , R13 , R14 , R15 , \
    L20 , L21 , L22 , R20 , R21 , R22) \
    { \
        {L00   , L01   , L02 , L03 , L04 , R00 , R01 , R02 , R03   , R04}   , \
        {L10   , L11   , L12 , L13 , L14 , R10 , R11 , R12 , R13   , R14}   , \
        {KC_NO , KC_NO , L20 , L21 , L22 , R20 , R21 , R22 , R05 , R15} , \
    }

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    KEYMAP(
		STN_S1,  STN_TL,  STN_PL,  STN_HL,  STN_ST1,  STN_ST3,  STN_FR,  STN_PR,  STN_LR,  STN_TR,  STN_DR,
		STN_S2,  STN_KL,  STN_WL,  STN_RL,  STN_ST2,  STN_ST4,  STN_RR,  STN_BR,  STN_GR,  STN_SR,  STN_ZR,
		STN_N1,   STN_A,   STN_O,   STN_E,    STN_U,  STN_N2),
};
