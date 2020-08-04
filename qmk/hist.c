// History management and output control
#include "hist.h"
#include "steno.h"
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "process_keycode/process_unicode_common.h"

history_t history[HIST_SIZE];
uint8_t hist_ind = 0;

void hist_add(history_t hist) {
    hist_ind ++;
    if (hist_ind == HIST_SIZE) {
        hist_ind = 0;
    }

    if (history[hist_ind].len) {
        free(history[hist_ind].search_nodes);
    }

#ifdef STENO_DEBUG_HIST
    steno_debug_ln("hist[%u]:", hist_ind);
    steno_debug_ln("  len: %u, repl_len: %u", hist.len, hist.repl_len);
    state_t state = hist.state;
    steno_debug_ln("  space: %u, cap: %u, glue: %u", state.space, state.cap, state.prev_glue);
    if (hist.output.type == RAW_STROKE) {
        char buf[24];
        stroke_to_string(hist.output.stroke, buf, NULL);
        steno_debug_ln("  output: %s", buf);
    } else {
        uint32_t node = hist.output.node;
        steno_debug_ln("  output: %lX", node);
    }
#endif
    history[hist_ind] = hist;
}

// Undo the last history entry. First delete the output, and then start from the initial state of the
// multi-stage input, and rebuild the output from there.
void hist_undo() {
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("hist_undo()");
#endif
    history_t hist = history[hist_ind];
    uint8_t len = hist.len;
    if (!len) {
        steno_error_ln("Invalid current history entry");
        tap_code(KC_BSPC);
        return;
    }

#ifdef STENO_DEBUG_HIST
    steno_debug_ln("  bspc len: %u", len);
#endif
    for (uint8_t i = 0; i < len; i ++) {
        tap_code(KC_BSPC);
    }
    state = hist.state;
    search_nodes_len = hist.search_nodes_len;
    memcpy(search_nodes, hist.search_nodes, search_nodes_len * sizeof(search_node_t));
    uint8_t hist_ind_save = hist_ind;
    for (uint8_t i = 0; i < hist.repl_len; i ++) {
        hist_ind = (hist_ind_save + i - hist.repl_len) % HIST_SIZE;
        history_t old_hist = history[hist_ind];
        assert((hist_ind & 0xE0) == 0);
#ifdef STENO_DEBUG_HIST
        steno_debug_ln("  hist_ind: %u", hist_ind);
#endif
        state = old_hist.state;
        if (!history[hist_ind].len) {
            history[hist_ind_save].len = 0;
            steno_error_ln("Invalid prev hist entry");
            return;
        }
        // `process_output` expects the previous history entry to be on `hist_ind`, but the
        // information for recreating the output is on the next history entry
        hist_ind = (hist_ind - 1) % HIST_SIZE;
        process_output(&state, old_hist.output, old_hist.repl_len);
    }
    hist_ind = hist_ind_save;

    if (hist_ind == 0) {
        hist_ind = HIST_SIZE - 1;
    } else {
        hist_ind --;
    }
}

uint16_t hex_to_keycode(uint8_t hex) {
    if (hex == 0x0) {
        return KC_0;
    } else if (hex < 0xA) {
        return KC_1 + (hex - 0x1);
    } else {
        return KC_A + (hex - 0xA);
    }
}

void register_hex32(uint32_t hex) {
    bool onzerostart = true;
    for (int i = 7; i >= 0; i--) {
        if (i <= 3) {
            onzerostart = false;
        }
        uint8_t digit = ((hex >> (i * 4)) & 0xF);
        if (digit == 0) {
            if (!onzerostart) {
                tap_code(hex_to_keycode(digit));
            }
        } else {
            tap_code(hex_to_keycode(digit));
            onzerostart = false;
        }
    }
}

#ifdef OLED_DRIVER_ENABLE
extern char last_trans[128];
extern uint8_t last_trans_size;
#endif
// Custom version of `send_string` that takes care of custom Unicode formats
uint8_t _send_unicode_string(char *buf, uint8_t len) {
    uint8_t str_len = 0;
    for (uint8_t i = 0; i < len; buf ++, i ++) {
        if (*buf == 1) {    // Custom unicode start byte
            uint32_t code_point = (uint32_t) buf[1] | (uint32_t) buf[2] << 8 | (uint32_t) buf[3] << 16;
#ifdef STENO_DEBUG_HIST
            steno_debug("<%lX>", code_point);
#endif
            tap_code16(LCTL(LSFT(KC_U)));
            register_hex32(code_point);
            tap_code(KC_ENT);
            buf += 3;
            i += 3;
        } else {
#ifdef OLED_DRIVER_ENABLE
            last_trans[last_trans_size++] = *buf;
#endif
            send_char(*buf);
        }
        str_len ++;
    }
    return str_len;
}

// Process the output. If it's a raw stroke (no nodes found for the input), then just output the stroke;
// otherwise, load the entry, perform the necessary transformations for capitalization, and output according
// to the bytes. Also takes care of outputting key codes and Unicode characters.
uint8_t process_output(state_t *state, output_t output, uint8_t repl_len) {
    // TODO optimization: compare beginning of current and string to replace
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("process_output()");
#endif
    int8_t counter = repl_len;
    while (counter > 0) {
        uint8_t old_hist_ind = (hist_ind - repl_len + counter) % HIST_SIZE;
        history_t old_hist = history[old_hist_ind];
#ifdef STENO_DEBUG_HIST
        steno_debug_ln("  old_hist_ind: %u, bspc len: %u", old_hist_ind, old_hist.len);
#endif
        if (!old_hist.len) {
            history[hist_ind].len = 0;
            steno_error_ln("Invalid prev hist entry");
            break;
        }
        for (uint8_t j = 0; j < old_hist.len; j ++) {
            tap_code(KC_BSPC);
        }
        counter -= old_hist.repl_len + 1;
    }

    state_t old_state = *state;
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("  old_state: space: %u, cap: %u, glue: %u", old_state.space, old_state.cap, old_state.prev_glue);
#endif
    uint8_t space = old_state.space, cap = old_state.cap;
    state->prev_glue = 0;
    state->space = 1;

    if (output.type == RAW_STROKE) {
        uint8_t len;
#ifdef STENO_DEBUG_HIST
        uint32_t stroke = output.stroke;
        steno_debug_ln("  stroke: %lX", stroke);
#endif
        if (stroke_to_string(output.stroke, _buf, &len)) {
            state->prev_glue = 1;
        }
#ifdef STENO_DEBUG_HIST
        steno_debug("  output: '");
#endif
        if (space && !(old_state.prev_glue && state->prev_glue)) {
            send_char(' ');
            steno_debug(" ");
            len ++;
        }
        send_string(_buf);
#ifdef STENO_DEBUG_HIST
        steno_debug_ln("%s'", _buf);
        steno_debug_ln("  -> %u", len);
#endif
        return len;
    }

    uint32_t node = output.node;
    seek(node);
    read_header();
    read_string();
    uint8_t entry_len = _header.entry_len;
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("  node: %lX, entry_len: %u", node, entry_len);
#endif

    attr_t attr = _header.attrs;
    state->cap = attr.caps;
    if (state->cap == ATTR_CAPS_KEEP) {
        state->cap = old_state.cap;
    }
    state->space = attr.space_after;
    state->prev_glue = attr.glue;
    space = space && attr.space_prev && entry_len && !(old_state.prev_glue && state->prev_glue);
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("  attr: glue: %u, cap: %u, str_only: %u", attr.glue, attr.caps, attr.str_only);
    steno_debug_ln("  output:");
#endif

    uint8_t has_raw_key = 0, str_len = 0;
    uint8_t mods = 0;
    for (uint8_t i = 0; i < entry_len; i ++) {
        if ((_buf[i] & 0x80) && !attr.str_only) {
            space = 0;
            has_raw_key = 1;
            uint8_t key_end = _buf[i] & 0x7F;
            i ++;
#ifdef STENO_DEBUG_HIST
            steno_debug("    keys: len: %u,", key_end);
#endif
            key_end += i;
            for ( ; i < key_end; i ++) {
#ifdef STENO_DEBUG_HIST
                steno_debug(" %02X", _buf[i]);
#endif
                if ((_buf[i] & 0xFC) == 0xE0) {
                    uint8_t mod_mask = 1 << (_buf[i] & 0x03);
                    if (mods & mod_mask) {
                        unregister_code(_buf[i]);
#ifdef STENO_DEBUG_HIST
                        steno_debug("^");
#endif
                    } else {
                        register_code(_buf[i]);
#ifdef STENO_DEBUG_HIST
                        steno_debug("v");
#endif
                    }
                    mods ^= mod_mask;
                } else {
                    tap_code(_buf[i]);
                }
            }
#ifdef STENO_DEBUG_HIST
            steno_debug_ln("");
#endif
        } else {
            uint8_t byte_len;
            if (attr.str_only) {
                byte_len = entry_len;
            } else {
                byte_len = _buf[i];
                i ++;
            }
            switch (cap) {
                case ATTR_CAPS_UPPER:
                    for (uint8_t j = 0; j < byte_len; j ++) {
                        _buf[j] = toupper(_buf[j]);
                    }
                    break;
                case ATTR_CAPS_CAPS:
                    _buf[i] = toupper(_buf[i]);
                    break;
            }

#ifdef STENO_DEBUG_HIST
            steno_debug("    str: '", str_len);
#endif
            cap = ATTR_CAPS_LOWER;
            if (space) {
#ifdef STENO_DEBUG_HIST
                steno_debug(" ");
#endif
                str_len ++;
                send_char(' ');
            }
            str_len += _send_unicode_string(_buf + i, byte_len);
#ifdef STENO_DEBUG_HIST
            steno_debug_ln("%s'", _buf + i);
#endif
            i += byte_len;
        }
    }
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("  -> %u", str_len);
#endif
    return has_raw_key ? 0 : str_len;
}
