#include <string.h>
#include "steno.h"
#include "keymap_steno.h"
#include "raw_hid.h"

#ifdef CUSTOM_STENO

#include "hist.h"

#ifdef USE_SPI_FLASH
#include "flash.h"
#else
#include "sdcard/fat.h"
#include "sdcard/partition.h"
#include "sdcard/sd_raw.h"
#endif

search_node_t search_nodes[SEARCH_NODES_SIZE];
uint8_t search_nodes_len = 0;
state_t state = {.space = 0, .cap = ATTR_CAPS_CAPS, .prev_glue = 0};

// Intercept the steno key codes, searches for the stroke, and outputs the output
bool send_steno_chord_user(steno_mode_t mode, uint8_t chord[6]) {
    uint32_t stroke = qmk_chord_to_stroke(chord);

    if (stroke == 0x1000) {  // Asterisk
        hist_undo();
        return false;
    }

    // TODO free up the next entry for boundary
    history_t new_hist;
    search_node_t *hist_nodes = malloc(search_nodes_len * sizeof(search_node_t));
    if (!hist_nodes) {
        xprintf("No memory for history!\n");
        return false;
    }
    memcpy(hist_nodes, search_nodes, search_nodes_len * sizeof(search_node_t));
    new_hist.search_nodes = hist_nodes;
    new_hist.search_nodes_len = search_nodes_len;

    uint32_t max_level_node = 0;
    uint8_t max_level = 0;
    search_on_nodes(search_nodes, &search_nodes_len, stroke, &max_level_node, &max_level);

    if (max_level_node) {
        new_hist.output.type = NODE_STRING;
        new_hist.output.node = max_level_node;
        new_hist.repl_len = max_level - 1;
    } else {
        new_hist.output.type = RAW_STROKE;
        new_hist.output.stroke = stroke;
        new_hist.repl_len = 0;
    }
    if (new_hist.repl_len) {
        state = history[(hist_ind - new_hist.repl_len + 1) % HIST_SIZE].state;
    }
    new_hist.state = state;
    steno_debug("steno(): state: space: %u, cap: %u, glue: %u\n", state.space, state.cap, state.prev_glue);
    new_hist.len = process_output(&state, new_hist.output, new_hist.repl_len);
    steno_debug("steno(): processed: state: space: %u, cap: %u, glue: %u\n", state.space, state.cap, state.prev_glue);
    if (new_hist.len) {
        hist_add(new_hist);
    }

    steno_debug("--------\n\n");
    return false;
}

#ifdef USE_SPI_FLASH
uint16_t crc8(uint8_t *data, uint8_t len) {
    uint8_t crc = 0;
    for (uint8_t i = 0; i < len; i ++) {
        crc ^= data[i];
        for (uint8_t i = 0; i < 8; ++i) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0x8C;
            } else {
                crc = (crc >> 1);
            }
        }
    }
    return crc;
}

#define nack(reason) \
    data[0] = 0x55; \
    data[1] = 0xFF; \
    data[2] = (reason); \
    data[PACKET_SIZE - 1] = crc8(data, MSG_SIZE); \
    raw_hid_send(data, PACKET_SIZE);

// Handle the HID packets, mostly for downloading and uploading the dictionary.
void raw_hid_receive(uint8_t *data, uint8_t length) {
    static mass_write_info_t mass_write_infos[PACKET_SIZE];
    static uint8_t mass_write_packet_num = 0;
    static uint8_t mass_write_packet_ind = 0;
    static uint32_t mass_write_addr = 0;

    if (mass_write_packet_num) {
        mass_write_info_t info = mass_write_infos[mass_write_packet_ind];
        uint8_t crc = crc8(data, info.len);
        if (crc != info.crc) {
            xprintf("calc: %X, info: %X\n", crc, info.crc);
            nack(0x04);
            return;
        }
        flash_write(mass_write_addr, data, info.len);
        mass_write_addr += info.len;
        mass_write_packet_ind ++;
        if (mass_write_packet_ind == mass_write_packet_num) {
            mass_write_packet_num = 0;
        }

        data[0] = 0x55;
        data[1] = 0x01;
        data[PACKET_SIZE - 1] = crc8(data, MSG_SIZE);
        raw_hid_send(data, PACKET_SIZE);
        return;
    }

    if (data[0] != 0xAA) {
        xprintf("head\n");
        nack(0x01);
        return;
    }

    uint8_t crc = crc8(data, MSG_SIZE);
    if (crc != data[PACKET_SIZE - 1]) {
        xprintf("CRC: %X\n", crc);
        nack(0x02);
        return;
    }

    uint32_t addr;
    uint8_t len;
    /* xprintf("head: %X\n", data[1]); */
    switch (data[1]) {
        case 0x01:;
            addr = (uint32_t) data[3] | (uint32_t) data[4] << 8 | (uint32_t) data[5] << 16;
            len = data[2];
            flash_write(addr, data + 6, len);
            data[0] = 0x55;
            data[1] = 0x01;
            break;
        case 0x02:;
            addr = (uint32_t) data[3] | (uint32_t) data[4] << 8 | (uint32_t) data[5] << 16;
            len = data[2];
            data[1] = 0x02;
            data[2] = len;
            flash_read(addr, data + 6, len);
            break;
        case 0x03:;
            addr = (uint32_t) data[3] | (uint32_t) data[4] << 8 | (uint32_t) data[5] << 16;
            flash_erase_page(addr);
            data[1] = 0x01;
            break;
        case 0x04:;
            mass_write_addr = (uint32_t) data[3] | (uint32_t) data[4] << 8 | (uint32_t) data[5] << 16;
            mass_write_packet_num = data[2];
            mass_write_packet_ind = 0;
            memcpy(mass_write_infos, data + 6, sizeof(mass_write_infos));
            data[1] = 0x01;
            break;
        default:;
            nack(0x03);
            return;
    }

    data[0] = 0x55;
    data[PACKET_SIZE - 1] = crc8(data, MSG_SIZE);
    raw_hid_send(data, PACKET_SIZE);
}
#else
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
#endif

// Setup the necessary stuff, init SD card or SPI flash. Delay so that it's easy for `hid-listen` to recognize
// the keyboard
void keyboard_post_init_user(void) {
    _delay_ms(2000);
#ifdef USE_SPI_FLASH
    flash_init();
#else
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

#ifdef OLED_DRIVER_ENABLE
    oled_set_contrast(0);
#endif

    return;
error:
    xprintf("Can't init\n");
    while(1);
#endif
}

void matrix_init_user() {
    steno_set_mode(STENO_MODE_GEMINI);
}

#endif
