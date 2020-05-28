#include "hist.h"
#include "quantum.h"
#include <string.h>
#include <ctype.h>

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

#if STENO_DEBUG
    xprintf("history[%u]:\n", hist_ind);
    xprintf("  len: %u\n", hist.len);
    xprintf("  repl_len: %u\n", hist.repl_len);
    state_t state = hist.state;
    xprintf("  state: space: %u, cap: %u, glue: %u\n", state.space, state.cap, state.prev_glue);
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
#if STENO_DEBUG
    xprintf("hist_undo()\n");
#endif
    history_t hist = history[hist_ind];
    uint8_t len = hist.len;
    if (!len) {
        xprintf("Invalid current history entry\n");
        tap_code(KC_BSPC);
        return;
    }

#if STENO_DEBUG
    xprintf("  bspc len: %u\n", len);
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
#if STENO_DEBUG
    xprintf("  hist_ind: %u\n", hist_ind);
#endif
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
#if STENO_DEBUG
    xprintf("process_output()\n");
#endif
    int8_t counter = repl_len;
    while (counter > 0) {
        uint8_t old_hist_ind = (hist_ind - repl_len + counter) % HIST_SIZE;
        history_t old_hist = history[old_hist_ind];
#if STENO_DEBUG
        xprintf("  old_hist_ind: %u\n", old_hist_ind);
        xprintf("  bspc len: %u\n", old_hist.len);
#endif
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
    uint8_t len = 0, space = old_state.space, cap = old_state.cap;
    state->cap = 0;
    state->prev_glue = 0;
    state->space = 1;

    if (output.type == RAW_STROKE) {
        if (stroke_to_string(output.stroke, _buf, &len)) {
            state->prev_glue = 1;
        }
        if (space) {
            send_char(' ');
            len ++;
        }
        send_string(_buf);
        return len;
    }

    uint8_t entry_len = _header.str_len;
    seek(output.node);
    read_header();
    read_string();

    attr_t attr = _header.attrs;
    switch (attr.caps) {
        case ATTR_CAPS_LOWER: state->cap = 0;             break;
        case ATTR_CAPS_UPPER: state->cap = 1;             break;
        case ATTR_CAPS_KEEP:  state->cap = old_state.cap; break;
        case ATTR_CAPS_CAPS:  state->cap = 1;             break;
    }
    space = space && attr.space_prev;
    state->space = attr.space_after;
    state->prev_glue = attr.glue;
#if STENO_DEBUG
    xprintf("  output:\n");
#endif
    if (space && entry_len && !(old_state.prev_glue && state->prev_glue)) {
#if STENO_DEBUG
        xprintf("    str: ' '\n");
#endif
        SEND_STRING(" ");
        len ++;
    }

    uint8_t has_raw_key = 0;
    for (uint8_t i = 0; i < entry_len; i ++) {
        if (_buf[i] & 0x80) {
            has_raw_key = 1;
            // TODO use mods
            i ++;
#if STENO_DEBUG
            xprintf("    key: %X\n", _buf[i]);
#endif
            tap_code(_buf[i]);
        } else {
            uint8_t str_end = _buf[i] + i;
#if STENO_DEBUG
            xprintf("    str: '");
            if (cap) {      // TODO handle ALL CAPS
                _buf[i + 1] = toupper(_buf[i + 1]);
                cap = 0;
            }
#endif
            for ( ; i < str_end; ) {
                i ++;
                len ++;
#if STENO_DEBUG
                xprintf("%c", _buf[i]);
#endif
                send_char(_buf[i]);
            }
#if STENO_DEBUG
            xprintf("'\n");
#endif
        }
    }
#if STENO_DEBUG
    xprintf("  -> %u\n", len);
#endif
    return has_raw_key ? 0 : len;
}
