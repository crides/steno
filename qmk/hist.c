#include "hist.h"
#include "quantum.h"
#include <string.h>

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
        tap_code(KC_BSPC);
        return;
    }
    // Check if replacements are available
    for (uint8_t i = 0; i < hist.repl_len; i ++) {
        history_t old_hist = history[(hist_ind + i - hist.repl_len) % HIST_SIZE];
        if (!old_hist.len) {
            history[hist_ind].len = 0;
            tap_code(KC_BSPC);
            return;
        }
    }

    for (uint8_t i = 0; i < len; i ++) {
        tap_code(KC_BSPC);
    }
    state = hist.state;
    search_nodes_len = hist.search_nodes_len;
    memcpy(search_nodes, hist.search_nodes, search_nodes_len * sizeof(search_node_t));
    for (uint8_t i = 0; i < hist.repl_len; i ++) {
        history_t old_hist = history[(hist_ind + i - hist.repl_len) % HIST_SIZE];
        state_t state = old_hist.state;
        process_output(&state, old_hist.output, old_hist.repl_len);
    }

    if (hist_ind == 0) {
        hist_ind = HIST_SIZE - 1;
    } else {
        hist_ind --;
    }
}

uint8_t process_output(state_t *state, output_t output, uint8_t repl_len) {
    // TODO use state
    for (uint8_t i = 0; i < repl_len; i ++) {
        history_t old_hist = history[(hist_ind + i + 1 - repl_len) % HIST_SIZE];
        for (uint8_t j = 0; j < old_hist.len; j ++) {
            tap_code(KC_BSPC);
        }
    }

    SEND_STRING(" ");
    if (output.type == RAW_STROKE) {
        char buf[24];
        stroke_to_string(output.stroke, buf);
        send_string(buf);
        return strlen(buf) + 1;
    }

    seek(output.node);
    read_header();
    read_string();
    send_string(_buf);
    return strlen(_buf) + 1;
}
