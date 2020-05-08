#include <string.h>
#include "steno.h"
#include "keymap_steno.h"
/* #include "action_layer.h" */
/* #include "eeconfig.h" */

#ifdef CUSTOM_STENO

#include "sdcard/fat.h"
#include "sdcard/partition.h"
#include "sdcard/sd_raw.h"

#include "stroke.h"
#include "hist.h"

search_node_t search_nodes[SEARCH_NODES_SIZE];
uint8_t search_node_size = 0;

bool send_steno_chord_user(steno_mode_t mode, uint8_t chord[6]) {
    uint32_t stroke = qmk_chord_to_stroke(chord);

    if (stroke == 0x1000) {  // Asterisk
        hist_undo();
        return false;
    }

    uint32_t max_level_node = 0;
    uint8_t max_level = 0, max_level_ended = 0;
    search_on_nodes(search_nodes, &search_node_size, stroke, &max_level_node, &max_level, &max_level_ended);

    if (max_level_node) {
        if (max_level > 1) {
            hist_replace(max_level - 1, max_level_node);
            hist_exec();
        } else {
            hist_add(max_level_node);
            hist_exec();
        }
    } else {
        // Perform default action
        hist_add_raw(stroke);
        hist_exec();
    }

    return false;
}

static uint8_t find_file_in_dir(struct fat_fs_struct* fs, struct fat_dir_struct* dd, const char* name, struct fat_dir_entry_struct* dir_entry) {
    while (fat_read_dir(dd, dir_entry)) {
        if (strcmp(dir_entry->long_name, name) == 0) {
            fat_reset_dir(dd);
            return 1;
        }
    }
    return 0;
}

static struct fat_file_struct* open_file_in_dir(struct fat_fs_struct* fs, struct fat_dir_struct* dd, const char* name) {
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
    if (!fs) {
        goto error;
    }

    /* open root directory */
    struct fat_dir_entry_struct directory;
    fat_get_dir_entry_of_path(fs, "/", &directory);

    struct fat_dir_struct *dd = fat_open_dir(fs, &directory);
    if (!dd) {
        goto error;
    }
    
    /* search file in current directory and open it */
    file = open_file_in_dir(fs, dd, "steno.bin");
    if (!file) {
        goto error;
    }

    search_nodes[0].node = 0;
    search_nodes[0].level = 0;
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
