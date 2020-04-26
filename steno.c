#include <string.h>
#include "steno.h"
#include "keymap_steno.h"
#include "action_layer.h"
#include "eeconfig.h"
#include "sdcard/fat.h"
#include "sdcard/partition.h"
#include "sdcard/sd_raw.h"

#ifdef CUSTOM_STENO
struct fat_file_struct *file;
int32_t _offset;
header_t _header;
child_t _child;
uint8_t _buf[128];

uint32_t search_nodes[8];
uint8_t search_node_size = 0;
uint32_t past_nodes[16];
uint8_t past_node_ind = 0;
history_node_t history[10];
uint8_t history_ind = 0;

void hist_inc(void) {
    if (history_ind == 99) {
        history_ind = 0;
    } else {
        history_ind ++;
    }
}

void hist_dec(void) {
    if (history_ind == 0) {
        history_ind = 99;
    } else {
        history_ind --;
    }
}

void hist_replace(uint8_t n, uint32_t node) {
    history_node_t new_hist;
    new_hist.input = 0;
    new_hist.node = node;
    new_hist.replaced = n;
    hist_inc();
    history[history_ind] = new_hist;
}

void hist_add(uint32_t node) {
    history_node_t new_hist;
    new_hist.replaced = 0;
    new_hist.input = 0;
    new_hist.node = node;
    hist_inc();
    history[history_ind] = new_hist;
}

void hist_add_raw(uint32_t input) {
    history_node_t new_hist;
    new_hist.replaced = 0;
    new_hist.input = input;
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
        read_string_at(cur.node);
        SEND_STRING(" ");
        send_string((char *) _buf);
    } else {    // Raw input
        SEND_STRING(" <RAW>");
    }
}

void hist_undo(void) {
    history_node_t cur = history[history_ind];
    hist_dec();
    if (cur.node) {
        read_header_at(cur.node);
        for (uint8_t i = 0; i <= _header.str_len; i ++) {   // Space included
            tap_code(KC_BSPC);
        }
    } else {    // Raw input
        tap_code(KC_BSPC);
        tap_code(KC_BSPC);
        tap_code(KC_BSPC);
        tap_code(KC_BSPC);
        tap_code(KC_BSPC);
        tap_code(KC_BSPC);
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

void print_stroke(uint32_t stroke) {
    uint8_t buf[24] = {0};
#define set_if_bit(c, n) buf[(22-n)] = stroke & ((uint32_t) 1 << n) ? c : '-';
    set_if_bit('#', 22);
    set_if_bit('S', 21);
    set_if_bit('T', 20);
    set_if_bit('K', 19);
    set_if_bit('P', 18);
    set_if_bit('W', 17);
    set_if_bit('H', 16);
    set_if_bit('R', 15);
    set_if_bit('A', 14);
    set_if_bit('O', 13);
    set_if_bit('*', 12);
    set_if_bit('E', 11);
    set_if_bit('U', 10);
    set_if_bit('F', 9);
    set_if_bit('R', 8);
    set_if_bit('P', 7);
    set_if_bit('B', 6);
    set_if_bit('L', 5);
    set_if_bit('G', 4);
    set_if_bit('T', 3);
    set_if_bit('S', 2);
    set_if_bit('D', 1);
    set_if_bit('Z', 0);
#undef set_if_bit
    uprintf("%s\n", buf);
}

void print_keys(uint32_t keys) {
    uint8_t buf[30] = {0};
#define set_if_bit(c, n) buf[(27-n)] = keys & ((uint32_t) 1 << n) ? c : '-';
    set_if_bit('N', 27);
    set_if_bit('n', 26);
    set_if_bit('S', 25);
    set_if_bit('s', 24);
    set_if_bit('T', 23);
    set_if_bit('K', 22);
    set_if_bit('P', 21);
    set_if_bit('W', 20);
    set_if_bit('H', 19);
    set_if_bit('R', 18);
    set_if_bit('A', 17);
    set_if_bit('O', 16);
    set_if_bit('%', 15);
    set_if_bit('^', 14);
    set_if_bit('&', 13);
    set_if_bit('*', 12);
    set_if_bit('E', 11);
    set_if_bit('U', 10);
    set_if_bit('F', 9);
    set_if_bit('R', 8);
    set_if_bit('P', 7);
    set_if_bit('B', 6);
    set_if_bit('L', 5);
    set_if_bit('G', 4);
    set_if_bit('T', 3);
    set_if_bit('S', 2);
    set_if_bit('D', 1);
    set_if_bit('Z', 0);
#undef set_if_bit
    uprintf("%s\n", buf);
}

bool send_steno_chord_user(steno_mode_t mode, uint8_t chord[6]) {
    uint32_t keys = ((uint32_t) chord[5] & 1)
        | ((uint32_t) chord[4] & 0x7F) << 1
        | ((uint32_t) chord[3] & 0x3F) << 8
        | ((uint32_t) chord[2] & 0x7F) << (14 - 2)
        | ((uint32_t) chord[1] & 0x7F) << 19
        | ((uint32_t) chord[0] & 0x30) << (26 - 4);

    uint32_t input = keys & 0xFFF;
    if (keys & 0xF000) {
        input |= 0x1000;
    }
    input |= (keys >> 3) & 0x1FE000;
    if (keys & 0x3000000) {
        input |= 0x200000;
    }
    if (keys & 0xC000000) {
        input |= 0x400000;
    }
    /* uprintf("%6lX ", input); */
    /* print_stroke(input); */

    uint32_t new_search_nodes[8];
    uint8_t new_search_node_size = 0;
    uint8_t max_level = 0;
    uint32_t max_level_node = 0;
    uint8_t max_level_ended = 0;
    /* uprintf("search_node_size = %u\n", search_node_size); */
    for (uint8_t i = 0; i < search_node_size; i ++) {
        /* uprintf("Searching on node %lu\n", search_nodes[i]); */
        uint32_t next_node = node_find_input(search_nodes[i], input);
        if (!next_node) {
            /* uprintf("  No path.\n"); */
            continue;
        }
        /* uprintf("  next node = %lu\n", next_node); */
        read_header_at(next_node);
        /* uprintf("    node_num = %u\n", _header.node_num); */
        /* uprintf("    level = %u\n", _header.level); */
        if (_header.str_len) {
            if (_header.level > max_level) {
                max_level = _header.level;
                max_level_node = next_node;
            }
            /* uint8_t buf[128] = {0}; */
            /* fat_read_file(file, buf, _header.str_len); */
            /* uprintf("    str: %s\n", buf); */
        }
        if (_header.node_num) {
            new_search_nodes[new_search_node_size] = next_node;
            new_search_node_size ++;
        } else {
            /* uprintf("  Path ended\n"); */
            if (next_node == max_level_node) {
                max_level_ended = 1;
            }
        }
    }

    /* uprintf("Final node: %lu %lX\n", max_level_node, max_level_node); */
    /* uprintf("  level: %u\n", max_level); */
    if (max_level_node) {
        read_string_at(max_level_node);
        if (max_level > 1) {
            /* uprintf("Replace %u with \"%s\"\n", max_level - 1, _buf); */
            hist_replace(max_level - 1, max_level_node);
            hist_exec();
        } else {
            /* uprintf("Append \"%s\"\n", _buf); */
            hist_add(max_level_node);
            hist_exec();
        }
    } else {
        // Perform default action
        if (input == 0x1000) {  // Asterisk
            hist_undo();
        } else {
            hist_add_raw(input);
            hist_exec();
        }
    }

    search_node_size = new_search_node_size + 1;
    search_nodes[0] = 0;
    if (max_level_ended) {
        /* uprintf("Max level ended; cleaning up\n"); */
    } else {
        /* uprintf("new_search_node_size: %u\n", new_search_node_size); */
        for (uint8_t i = 0; i < new_search_node_size; i ++) {
            /* uprintf("new_search_node: %lu %lX\n", new_search_nodes[i], new_search_nodes[i]); */
            search_nodes[i + 1] = new_search_nodes[i];
        }
    }

    return false;
}

void read_file_at(int32_t addr, void *dest, uint16_t size) {
    fat_seek_file(file, &addr, FAT_SEEK_SET);
    fat_read_file(file, dest, size);
}

void read_string_at(int32_t addr) {
    read_header_at(addr);
    fat_read_file(file, _buf, _header.str_len);
    _buf[_header.str_len] = 0;
}

void read_header_at(int32_t addr) {
    fat_seek_file(file, &addr, FAT_SEEK_SET);
    fat_read_file(file, (uint8_t *) &_header, sizeof(header_t));
}

void read_child_at(int32_t addr) {
    fat_seek_file(file, &addr, FAT_SEEK_SET);
    fat_read_file(file, (uint8_t *) &_child, sizeof(child_t));
}

uint32_t node_find_input(uint32_t header_ptr, uint32_t input) {
    read_header_at(header_ptr);
    uint32_t children_ptr = header_ptr + sizeof(header_t) + _header.str_len;
    uint16_t start = 0, end = _header.node_num;
    /* uprintf("%u %u %u\n", _header.node_num, _header.level, _header.str_len); */
    while (start != end) {
        uint16_t mid_ind = (start + end) / 2;
        uint32_t mid_child_ptr = (uint32_t) mid_ind * sizeof(child_t) + children_ptr;
        read_child_at(mid_child_ptr);
        /* uprintf("%u %u %u\n", start, mid_ind, end); */
        /* uprintf("%lX %lX\n", _child.input, _child.addr); */
        if (input == _child.input) {
            return _child.addr;
        }
        if (input < _child.input) {
            end = mid_ind;
        } else {
            start = mid_ind + 1;
        }
    }
    return 0;
}

uint8_t find_file_in_dir(struct fat_fs_struct* fs, struct fat_dir_struct* dd, const char* name, struct fat_dir_entry_struct* dir_entry) {
    while (fat_read_dir(dd, dir_entry)) {
        if (strcmp(dir_entry->long_name, name) == 0) {
            fat_reset_dir(dd);
            return 1;
        }
    }
    return 0;
}

struct fat_file_struct* open_file_in_dir(struct fat_fs_struct* fs, struct fat_dir_struct* dd, const char* name) {
    struct fat_dir_entry_struct file_entry;
    if (!find_file_in_dir(fs, dd, name, &file_entry)) {
        return 0;
    }
    return fat_open_file(fs, &file_entry);
}

void keyboard_post_init_user(void) {
    _delay_ms(2000);
    if (!sd_raw_init()) {
        goto error;
    }

    struct partition_struct *partition = partition_open();
    if (!partition) {
        goto error;
    }

    /* open file system */
    struct fat_fs_struct *fs = fat_open(partition);
    if(!fs) {
        goto error;
    }

    /* open root directory */
    struct fat_dir_entry_struct directory;
    fat_get_dir_entry_of_path(fs, "/", &directory);

    struct fat_dir_struct *dd = fat_open_dir(fs, &directory);
    if(!dd) {
        goto error;
    }
    
    /* search file in current directory and open it */
    file = open_file_in_dir(fs, dd, "steno.bin");
    if(!file) {
        goto error;
    }

    search_nodes[0] = 0;
    search_node_size = 1;
    return;
error:
    uprintf("Can't init\n");
    while(1);
}

void matrix_init_user() {
    steno_set_mode(STENO_MODE_GEMINI);
}
#endif
