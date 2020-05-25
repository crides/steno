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
        state_t state = old_hist.state;
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
    uint8_t len = 0, space = old_state.space;
    state->cap = 0;
    state->prev_glue = 0;
    state->space = 1;

    if (output.type == RAW_STROKE) {
        if (stroke_to_string(output.stroke, _buf, &len)) {
            state->prev_glue = 1;
        }
    } else {
        seek(output.node);
        read_header();
        read_string();
        len = strlen(_buf);

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
        // TODO use other state/attrs
        if (old_state.cap) {
            _buf[0] = toupper(_buf[0]);
        }
    }
#if STENO_DEBUG
    xprintf("  output: '");
#endif
    if (space && len && !(old_state.prev_glue && state->prev_glue)) {
#if STENO_DEBUG
        xprintf(" ");
#endif
        SEND_STRING(" ");
        len ++;
    }

#if STENO_DEBUG
    xprintf("%s'\n", _buf);
    xprintf("  -> %u\n", len);
#endif
    send_string(_buf);
    return len;
}
