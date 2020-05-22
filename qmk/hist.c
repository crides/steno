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

    history[hist_ind] = hist;
}

void hist_undo() {
    history_t hist = history[hist_ind];
    uint8_t len = hist.len;
    if (!len) {
        xprintf("Invalid current history entry\n");
        tap_code(KC_BSPC);
        return;
    }

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
    int8_t counter = repl_len;
    while (counter > 0) {
        uint8_t old_hist_ind = (hist_ind - repl_len + counter) % HIST_SIZE;
        history_t old_hist = history[old_hist_ind];
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
            case ATTR_CAPS_KEEP:        state->cap = old_state.cap; break;
            case ATTR_CAPS_CAPS: state->cap = 1;             break;
        }
        space = space && attr.space_prev;
        state->space = attr.space_after;
        state->prev_glue = state->prev_glue && attr.glue;
        // TODO use other state/attrs
        if (old_state.cap) {
            _buf[0] = toupper(_buf[0]);
        }
    }
    if (space && !(old_state.prev_glue && state->prev_glue)) {
        SEND_STRING(" ");
        len ++;
    }

    send_string(_buf);
    return len;
}
