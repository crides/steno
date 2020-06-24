#include "hist.h"
#include "steno.h"
#include <string.h>
#include <ctype.h>
#include <assert.h>

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

#ifdef STENO_DEBUG
    xprintf("hist[%u]:\n", hist_ind);
    xprintf("  len: %u, repl_len: %u\n", hist.len, hist.repl_len);
    state_t state = hist.state;
    xprintf("  space: %u, cap: %u, glue: %u\n", state.space, state.cap, state.prev_glue);
    if (hist.output.type == RAW_STROKE) {
        char buf[24];
        uint8_t _len = 0;
        stroke_to_string(hist.output.stroke, buf, &_len);
        xprintf("  output: %s\n", buf);
    } else {
        uint32_t node = hist.output.node;
        xprintf("  output: %lX\n", node);
    }
#endif
    history[hist_ind] = hist;
}

void hist_undo() {
    steno_debug("hist_undo()\n");
    history_t hist = history[hist_ind];
    uint8_t len = hist.len;
    if (!len) {
        xprintf("Invalid current history entry\n");
        tap_code(KC_BSPC);
        return;
    }

    steno_debug("  bspc len: %u\n", len);
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
        steno_debug("  hist_ind: %u\n", hist_ind);
        state = old_hist.state;
        if (!history[hist_ind].len) {
            history[hist_ind_save].len = 0;
            xprintf("Invalid previous history entry\n");
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

uint8_t process_output(state_t *state, output_t output, uint8_t repl_len) {
    // TODO optimization: compare beginning of current and string to replace
    steno_debug("process_output()\n");
    int8_t counter = repl_len;
    while (counter > 0) {
        uint8_t old_hist_ind = (hist_ind - repl_len + counter) % HIST_SIZE;
        history_t old_hist = history[old_hist_ind];
        steno_debug("  old_hist_ind: %u, bspc len: %u\n", old_hist_ind, old_hist.len);
        if (!old_hist.len) {
            history[hist_ind].len = 0;
            xprintf("Invalid previous history entry\n");
            break;
        }
        for (uint8_t j = 0; j < old_hist.len; j ++) {
            tap_code(KC_BSPC);
        }
        counter -= old_hist.repl_len + 1;
    }

    state_t old_state = *state;
    steno_debug("  old_state: space: %u, cap: %u, glue: %u\n", old_state.space, old_state.cap, old_state.prev_glue);
    uint8_t space = old_state.space, cap = old_state.cap;
    state->prev_glue = 0;
    state->space = 1;

    if (output.type == RAW_STROKE) {
        uint8_t len;
        steno_debug("  stroke: %lX\n", output.stroke);
        if (stroke_to_string(output.stroke, _buf, &len)) {
            state->prev_glue = 1;
        }
        steno_debug("  output: '");
        if (space && !(old_state.prev_glue && state->prev_glue)) {
            send_char(' ');
            steno_debug(" ");
            len ++;
        }
        send_string(_buf);
        steno_debug("%s'\n", _buf);
        steno_debug("  -> %u\n", len);
        return len;
    }

    uint32_t node = output.node;
    seek(node);
    read_header();
    read_string();
    uint8_t entry_len = _header.entry_len;
    steno_debug("  node: %lX, entry_len: %u\n", node, entry_len);

    attr_t attr = _header.attrs;
    state->cap = attr.caps;
    if (state->cap == ATTR_CAPS_KEEP) {
        state->cap = old_state.cap;
    }
    state->space = attr.space_after;
    state->prev_glue = attr.glue;
    space = space && attr.space_prev && entry_len && !(old_state.prev_glue && state->prev_glue);
    steno_debug("  attr: glue: %u, cap: %u, str_only: %u\n", attr.glue, attr.caps, attr.str_only);
    steno_debug("  output:\n");

    uint8_t has_raw_key = 0, len = 0;
    uint8_t mods = 0;
    for (uint8_t i = 0; i < entry_len; i ++) {
        if ((_buf[i] & 0x80) && !attr.str_only) {
            space = 0;
            has_raw_key = 1;
            uint8_t key_end = _buf[i] & 0x7F;
            i ++;
            steno_debug("    keys: len: %u,", key_end);
            key_end += i;
            for ( ; i < key_end; i ++) {
                steno_debug(" %02X", _buf[i]);
                if ((_buf[i] & 0xFC) == 0xE0) {
                    uint8_t mod_mask = 1 << (_buf[i] & 0x03);
                    if (mods & mod_mask) {
                        unregister_code(_buf[i]);
                        steno_debug("^");
                        mods &= ~mod_mask;
                    } else {
                        register_code(_buf[i]);
                        steno_debug("v");
                        mods |= mod_mask;
                    }
                } else {
                    tap_code(_buf[i]);
                }
            }
            steno_debug("\n");
        } else {
            if (attr.str_only) {
                len = entry_len;
            } else {
                len = _buf[i];
                i ++;
            }
            steno_debug("    str: len: %u, '", len);
            switch (cap) {
                case ATTR_CAPS_UPPER:
                    for (uint8_t j = 0; j < len; j ++) {
                        _buf[j] = toupper(_buf[j]);
                    }
                    break;
                case ATTR_CAPS_CAPS:
                    _buf[i] = toupper(_buf[i]);
                    break;
            }
            cap = ATTR_CAPS_LOWER;
            if (space) {
                steno_debug(" ");
                len ++;
                send_char(' ');
            }
            steno_debug("%s", _buf);
            send_string(_buf + i);
            i += len;
            steno_debug("'\n");
        }
    }
    steno_debug("  -> %u\n", len);
    return has_raw_key ? 0 : len;
}
