#include <avr/io.h>
#include "steno.h"
#include "keymap_steno.h"

void keyboard_post_init_user(void) {
    steno_set_mode(STENO_MODE_GEMINI);
    DDRE |= _BV(DDE6);
    PORTE |= _BV(PORTE6);
    ebd_steno_init();
    _delay_ms(30);
    PORTE &= ~_BV(PORTE6);
}

uint32_t qmk_chord_to_stroke(const uint8_t chord[6]) {
    const uint32_t keys = ((uint32_t) chord[5] & 1)
        | ((uint32_t) chord[4] & 0x7F) << 1
        | ((uint32_t) chord[3] & 0x3F) << 8
        | ((uint32_t) chord[2] & 0x7F) << (14 - 2)
        | ((uint32_t) chord[1] & 0x7F) << 19
        | ((uint32_t) chord[0] & 0x30) << (26 - 4);

    uint32_t stroke = keys & 0xFFF;
    if (keys & 0xF000) {    // Asterisk
        stroke |= 0x1000;
    }
    stroke |= (keys >> 3) & 0x1FE000; // Left side of asterisk
    if (keys & 0x3000000) { // S-
        stroke |= 0x200000;
    }
    if (keys & 0xC000000) { // #
        stroke |= 0x400000;
    }
    return stroke;
}
 
bool send_steno_chord_user(steno_mode_t mode, uint8_t chord[6]) {
    static bool embedded = false;
    const uint32_t stroke = qmk_chord_to_stroke(chord);
    steno_debug_ln("convert %06lX", stroke);
    if (stroke == 0x453842) {       // #PHO*EBD
        embedded = !embedded;
        if (embedded) {
            PORTE |= _BV(PORTE6);
        } else {
            PORTE &= ~_BV(PORTE6);
        }
        return false;
    }
    if (embedded) {
        ebd_steno_process_stroke(stroke);
        return false;
    } else {
        return true;
    }
}
