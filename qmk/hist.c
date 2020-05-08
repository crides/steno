#include "hist.h"
#include "stroke.h"
#include "quantum.h"
#include <string.h>

history_node_t history[HIST_SIZE];
uint8_t history_ind = 0;

void hist_inc(void) {
    if (history_ind == (HIST_SIZE - 1)) {
        history_ind = 0;
    } else {
        history_ind ++;
    }
}

void hist_dec(void) {
    if (history_ind == 0) {
        history_ind = HIST_SIZE - 1;
    } else {
        history_ind --;
    }
}

void hist_replace(uint8_t n, uint32_t node) {
    history_node_t new_hist;
    new_hist.stroke = 0;
    new_hist.node = node;
    new_hist.replaced = n;
    hist_inc();
    history[history_ind] = new_hist;
}

void hist_add(uint32_t node) {
    history_node_t new_hist;
    new_hist.replaced = 0;
    new_hist.stroke = 0;
    new_hist.node = node;
    hist_inc();
    history[history_ind] = new_hist;
}

void hist_add_raw(uint32_t stroke) {
    history_node_t new_hist;
    new_hist.replaced = 0;
    new_hist.stroke = stroke;
    new_hist.node = 0;
    hist_inc();
    history[history_ind] = new_hist;
}

void hist_exec() {  // Execute the current history entry
    history_node_t cur = history[history_ind];
    if (cur.replaced) {
        hist_dec();
        for (uint8_t i = 0; i < cur.replaced; i ++) {
            hist_undo();
        }
        for (uint8_t i = 0; i < cur.replaced; i ++) {
            hist_inc();
        }
        hist_inc();
        /* history[history_ind] = cur; */
    }
    if (cur.node) {
        seek(cur.node);
        read_header();
        read_string();
        SEND_STRING(" ");
        send_string((char *) _buf);
    } else {    // Raw input
        char buf[30];
        stroke_to_string(cur.stroke, buf);
        SEND_STRING(" ");
        send_string((char *) buf);
    }
}

void hist_undo(void) {
    history_node_t cur = history[history_ind];
    hist_dec();
    if (cur.node) {
        seek(cur.node);
        read_header();
        for (uint8_t i = 0; i <= _header.str_len; i ++) {   // Space included
            tap_code(KC_BSPC);
        }
    } else {    // Raw input
        char buf[30];
        stroke_to_string(cur.stroke, buf);
        uint8_t len = strlen(buf) + 1;
        for (uint8_t i = 0; i < len; i ++) {
            tap_code(KC_BSPC);
        }
    }

    if (cur.replaced) {     // Redo previous entries
        for (uint8_t i = 0; i < cur.replaced; i ++) {
            hist_dec();
        }
        for (uint8_t i = 0; i < cur.replaced; i ++) {
            hist_inc();
            hist_exec();
        }
    }
}

