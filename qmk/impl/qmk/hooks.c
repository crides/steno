#include "steno.h"

void keyboard_post_init_user(void) {
    ebd_steno_init();
}

bool send_steno_chord_user(const steno_mode_t mode, const uint8_t chord[6]) {
    const uint32_t stroke = qmk_chord_to_stroke(chord);
    ebd_steno_process_stroke(stroke);
    return false;
}
