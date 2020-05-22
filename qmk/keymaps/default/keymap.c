#include <string.h>
#include "steno.h"
#include "keymap_steno.h"
#include "action_layer.h"
#include "eeconfig.h"
#include "sdcard/fat.h"
#include "sdcard/partition.h"
#include "sdcard/sd_raw.h"

// Layers
#define _STENO  0
#define _QWERTY 1
#define _QWERTY_STENO 2

#define KEYMAP( \
    L00 , L01 , L02 , L03 , L04 , R00    , R01 , R02 , R03 , R04 , R05 , \
    L10 , L11 , L12 , L13 , L14 , R10    , R11 , R12 , R13 , R14 , R15 , \
    L20 , L21 , L22 , R20 , R21 , R22) \
    { \
        {L00   , L01   , L02 , L03 , L04 , R00 , R01 , R02 , R03   , R04}   , \
        {L10   , L11   , L12 , L13 , L14 , R10 , R11 , R12 , R13   , R14}   , \
        {KC_NO , KC_NO , L20 , L21 , L22 , R20 , R21 , R22 , R05 , R15} , \
    }

/* enum combos { */
/*     COMBO_A, */
/*     COMBO_S, */
/*     COMBO_D, */
/*     COMBO_F, */
/*     COMBO_G, */
/*     COMBO_H, */
/*     COMBO_J, */
/*     COMBO_K, */
/*     COMBO_L, */
/*     COMBO_SCLN, */
/* }; */

/* const uint16_t PROGMEM combo_a[] = {KC_Q, KC_Z, COMBO_END}; */
/* const uint16_t PROGMEM combo_s[] = {KC_W, KC_X, COMBO_END}; */
/* const uint16_t PROGMEM combo_d[] = {KC_E, KC_C, COMBO_END}; */
/* const uint16_t PROGMEM combo_f[] = {KC_R, KC_V, COMBO_END}; */
/* const uint16_t PROGMEM combo_g[] = {KC_T, KC_B, COMBO_END}; */
/* const uint16_t PROGMEM combo_h[] = {KC_Y, KC_N, COMBO_END}; */
/* const uint16_t PROGMEM combo_j[] = {KC_U, KC_M, COMBO_END}; */
/* const uint16_t PROGMEM combo_k[] = {KC_I, KC_COMM, COMBO_END}; */
/* const uint16_t PROGMEM combo_l[] = {KC_O, KC_DOT, COMBO_END}; */
/* const uint16_t PROGMEM combo_scln[] = {KC_P, KC_SLSH, COMBO_END}; */

/* combo_t key_combos[COMBO_COUNT] = { */
/*   [COMBO_A   ] = COMBO(combo_a   , KC_A), */
/*   [COMBO_S   ] = COMBO(combo_s   , KC_S), */
/*   [COMBO_D   ] = COMBO(combo_d   , KC_D), */
/*   [COMBO_F   ] = COMBO(combo_f   , KC_F), */
/*   [COMBO_G   ] = COMBO(combo_g   , KC_G), */
/*   [COMBO_H   ] = COMBO(combo_h   , KC_H), */
/*   [COMBO_J   ] = COMBO(combo_j   , KC_J), */
/*   [COMBO_K   ] = COMBO(combo_k   , KC_K), */
/*   [COMBO_L   ] = COMBO(combo_l   , KC_L), */
/*   [COMBO_SCLN] = COMBO(combo_scln, KC_SCLN), */
/* }; */

#define DEL MT(MOD_LGUI, KC_DEL)
#define TAB MT(MOD_LCTL, KC_TAB)
#define ESC LT(_SYMBOL2, KC_ESC)
#define ENT LT(_SYMBOL1, KC_ENT)
#define SPC MT(MOD_LSFT, KC_SPC)
#define BKSP MT(MOD_LALT, KC_BSPC)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_STENO] = KEYMAP(
		STN_S1         , STN_TL , STN_PL , STN_HL , STN_ST1 , STN_ST3 , STN_FR  , STN_PR , STN_LR  , STN_TR , STN_DR  ,
		STN_S2         , STN_KL , STN_WL , STN_RL , STN_ST2 , STN_ST4 , STN_RR  , STN_BR , STN_GR  , STN_SR , STN_ZR  ,
		STN_N1         , STN_A  , STN_O  , STN_E  , STN_U   , STN_N2) ,

    /* [_QWERTY] = KEYMAP( */
		/* KC_Q  , KC_W , KC_E , KC_R , KC_T , KC_Y   , KC_U , KC_I    , KC_O   , KC_P    , TG(_QWERTY) , */
		/* KC_Z  , KC_X , KC_C , KC_V , KC_B , KC_N   , KC_M , KC_COMM , KC_DOT , KC_SLSH , KC_NO, */
		/* KC_NO , KC_C , KC_V , KC_N , KC_M , KC_NO) , */

/*     [_QWERTY_STENO] = KEYMAP( */
/* 		KC_Q           , KC_W   , KC_E   , KC_R   , KC_T    , KC_Y    , KC_U   , KC_I    , KC_O   , KC_P    , TG(_QWERTY_STENO) , */
/* 		KC_A           , KC_S   , KC_D   , KC_F   , KC_G    , KC_H    , KC_J   , KC_K , KC_L , KC_SCLN , KC_DQUO , */
/* 		KC_NO           , KC_C   , KC_V   , KC_N   , KC_M    , KC_NO)   , */
};
