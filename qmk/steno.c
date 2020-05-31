#include <string.h>
#include "steno.h"
#include "keymap_steno.h"
#include "raw_hid.h"
/* #include "action_layer.h" */
/* #include "eeconfig.h" */

#ifdef CUSTOM_STENO

#include "hist.h"
#include "flash.h"

search_node_t search_nodes[SEARCH_NODES_SIZE];
uint8_t search_nodes_len = 0;
state_t state = {.space = 0, .cap = 1, .prev_glue = 0};

/* bool send_steno_chord_user(steno_mode_t mode, uint8_t chord[6]) { */
/*     uint32_t stroke = qmk_chord_to_stroke(chord); */

/*     if (stroke == 0x1000) {  // Asterisk */
/*         hist_undo(); */
/*         return false; */
/*     } */

/*     // TODO free up the next entry for boundary */
/*     history_t new_hist; */
/*     search_node_t *hist_nodes = malloc(search_nodes_len * sizeof(search_node_t)); */
/*     if (!hist_nodes) { */
/*         xprintf("No memory for history!\n"); */
/*         return false; */
/*     } */
/*     memcpy(hist_nodes, search_nodes, search_nodes_len * sizeof(search_node_t)); */
/*     new_hist.search_nodes = hist_nodes; */
/*     new_hist.search_nodes_len = search_nodes_len; */

/*     uint32_t max_level_node = 0; */
/*     uint8_t max_level = 0; */
/*     search_on_nodes(search_nodes, &search_nodes_len, stroke, &max_level_node, &max_level); */

/*     if (max_level_node) { */
/*         new_hist.output.type = NODE_STRING; */
/*         new_hist.output.node = max_level_node; */
/*         new_hist.repl_len = max_level - 1; */
/*     } else { */
/*         new_hist.output.type = RAW_STROKE; */
/*         new_hist.output.stroke = stroke; */
/*         new_hist.repl_len = 0; */
/*     } */
/*     if (new_hist.repl_len) { */
/*         state = history[(hist_ind - new_hist.repl_len + 1) % HIST_SIZE].state; */
/*     } */
/*     new_hist.state = state; */
/*     steno_debug("steno(): state: space: %u, cap: %u, glue: %u\n", state.space, state.cap, state.prev_glue); */
/*     new_hist.len = process_output(&state, new_hist.output, new_hist.repl_len); */
/*     steno_debug("steno(): processed: state: space: %u, cap: %u, glue: %u\n", state.space, state.cap, state.prev_glue); */
/*     if (new_hist.len) { */
/*         hist_add(new_hist); */
/*     } */

/*     steno_debug("--------\n\n"); */
/*     return false; */
/* } */

void keyboard_post_init_user(void) {
    _delay_ms(2000);
    flash_init();
    xprintf("init\n");
}

void matrix_init_user() {
    steno_set_mode(STENO_MODE_GEMINI);
}

uint16_t crc16(uint8_t *data, uint8_t len) {
    uint16_t crc = 0xFFFF;
    for (uint8_t i = 0; i < len; i ++) {
        crc ^= data[i];
        for (uint8_t i = 0; i < 8; ++i) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = (crc >> 1);
            }
        }
    }
    return crc;
}

void raw_hid_receive(uint8_t *data, uint8_t length) {
    /* xprintf("recv: "); */
    /* for (uint8_t i = 0; i < 24; i ++) { */
    /*     xprintf(" %02X", data[i]); */
    /* } */
    /* xprintf("\n"); */
    /* static uint16_t time = 0; */

    /* xprintf("from last: %u\n\n", timer_elapsed(time)); */
    /* time = timer_read(); */
    if (data[0] != 0xAA) {
        xprintf("head\n");
        return;
    }
    uint16_t crc = crc16(data, 22);
    if (crc != ((uint16_t) data[22] | (uint16_t) data[23] << 8)) {
        xprintf("CRC: %X\n", crc);
        return;
    }
    /* xprintf("after crc: %u\n", timer_elapsed(time)); */
    uint32_t addr;
    uint8_t len;
    switch (data[1]) {
        case 0x01:;
            addr = (uint32_t) data[3] | (uint32_t) data[4] << 8 | (uint32_t) data[5] << 16;
            flash_write(addr, data + 6, data[2]);
            memset(data, 0, 32);
            data[0] = 0x55;
            data[1] = 0x01;
            break;
        case 0x02:;
            addr = (uint32_t) data[3] | (uint32_t) data[4] << 8 | (uint32_t) data[5] << 16;
            len = data[2];
            memset(data, 0, 32);
            data[0] = 0x55;
            data[1] = 0x02;
            data[2] = len;
            flash_read(addr, data + 6, len);
            break;
        case 0x03:;
            addr = (uint32_t) data[3] | (uint32_t) data[4] << 8 | (uint32_t) data[5] << 16;
            flash_erase_page(addr);
            memset(data, 0, 32);
            data[0] = 0x55;
            data[1] = 0x01;
            break;
    }
    /* xprintf("after op: %u\n", timer_elapsed(time)); */
    crc = crc16(data, 22);
    data[22] = crc & 0xFF;
    data[23] = (crc >> 8) & 0xFF;
    raw_hid_send(data, 32);
    /* xprintf("after send: %u\n", timer_elapsed(time)); */
}

#endif
