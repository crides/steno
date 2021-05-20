#include "steno.h"

void keyboard_post_init_user(void) {
    ebd_steno_init();
}

static bool steno_key_valid(const uint16_t key) {
    return key >= STN__Z && key <= STN_NUM;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint32_t pressed = 0;
    static uint32_t current = 0;
    if (steno_key_valid(keycode)) {
        const uint8_t key = keycode - STN__Z;
        if (record->event.pressed) {
            pressed |= (uint32_t) 1 << key;
            current |= (uint32_t) 1 << key;
        } else {
            current &= ~((uint32_t) 1 << key);
            if (current == 0) {
                ebd_steno_process_stroke(pressed);
                pressed = 0;
            }
        }
        return false;
    } else {
        return true;
    }
}

