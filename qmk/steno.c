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

void raw_hid_receive(uint8_t *data, uint8_t length) {
    xprintf("recv: ");
    xprintf("  length: %u\n  ", length);
    for (uint8_t i = 0; i < length; i ++) {
        xprintf(" %02X", data[i]);
    }
    xprintf("\n");
    uint32_t addr = (uint32_t) data[0] | (uint32_t) data[1] << 8 | (uint32_t) data[2] << 16;
    flash_write(addr, data + 4, data[3]);
    flash_read(addr, data + 4, data[3]);
    raw_hid_send(data, 32);
    xprintf("Sent?!\n");
}

#endif
