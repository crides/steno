#include <string.h>
#include "steno.h"
#include "keymap_steno.h"
#include "raw_hid.h"
#include "flash.h"
#include <stdio.h>

#ifdef CUSTOM_STENO

#include "hist.h"
/* #include "analog.h" */

#ifndef __AVR__
#include "adc.h"
#include "app_ble_func.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_log.h"
#include "nrf_gpio.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_ble_gatt.h"

void tap_code(uint8_t code) {
    register_code(code);
    unregister_code(code);
}

void tap_code16(uint16_t code) {
    register_code16(code);
    unregister_code16(code);
}
#endif

search_node_t search_nodes[SEARCH_NODES_SIZE];
uint8_t search_nodes_len = 0;
state_t state = {.space = 0, .cap = ATTR_CAPS_CAPS, .prev_glue = 0};
#ifdef OLED_DRIVER_ENABLE
char last_stroke[24];
char last_trans[128];
uint8_t last_trans_size;
#endif
#ifndef __AVR__
static bt_state_t bt_state = BT_ACTIVE;
static uint32_t bt_state_time;
#endif

// Intercept the steno key codes, searches for the stroke, and outputs the output
bool send_steno_chord_user(steno_mode_t mode, uint8_t chord[6]) {
    bt_state = BT_ACTIVE;
    bt_state_time = timer_read32();

    uint32_t stroke = qmk_chord_to_stroke(chord);
#ifdef OLED_DRIVER_ENABLE
    last_trans_size = 0;
    memset(last_trans, 0, 128);
    stroke_to_string(stroke, last_stroke, NULL);
#endif

    extern int usbd_send_consumer(uint16_t data);
    if (stroke == 0x1000) {  // Asterisk
        hist_undo();
        tap_code(KC_VOLU);
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
            crc = crc >> 1;
            if (crc & 1) {
                crc = crc ^ 0x8C;
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
#endif

// Setup the necessary stuff, init SD card or SPI flash. Delay so that it's easy for `hid-listen` to recognize
// the keyboard
void keyboard_post_init_user(void) {
#ifdef USE_SPI_FLASH
    flash_init();
#else
    extern FATFS fat_fs;
    if (pf_mount(&fat_fs)) {
        xprintf("Volume\n");
        goto error;
    }

    FRESULT res = pf_open("STENO.BIN");
    if (res) {
        xprintf("File: %X\n", res);
        goto error;
    }

    xprintf("init\n");
    return;
error:
    xprintf("Can't init\n");
    while(1);
#endif
    
#ifndef __AVR__
    nrf_gpio_cfg_input(BUTTON, NRF_GPIO_PIN_PULLUP);
    bt_state_time = timer_read32();
#endif
}

void matrix_init_user() {
    steno_set_mode(STENO_MODE_GEMINI);
}

#ifdef OLED_DRIVER_ENABLE
void oled_task_user(void) {
#ifndef __AVR__
    static uint8_t button_last;
    static uint8_t device_id = 0;
    static disp_state_t state;
    static uint32_t status_time = 0;
    static uint32_t status_button_time = 0;
#endif

    if (bt_state != BT_ACTIVE) {
        nrf_pwr_mgmt_run();
    } else {
        if (timer_elapsed32(bt_state_time) > BT_ACTIVE_HOLD_TIME) {
            NRF_LOG_INFO("Going into idle");
            bt_state = BT_IDLE;
        }
    }

    oled_set_contrast(0);
    char buf[32];
#ifdef __AVR__
    uint16_t adc = analogReadPin(B5);
    uint16_t volt = (uint32_t) adc * 33 * 2 * 10 / 1024;
    sprintf(buf, "BAT: %u.%uV\n", volt / 100, volt % 100);
#else
    uint8_t button = !nrf_gpio_pin_read(BUTTON);
    switch (state) {
        case DISP_NORMAL:
            if (button && !button_last) {
                state = DISP_STATUS;
                status_time = timer_read32();
            }
            oled_write_ln(last_stroke, false);
            oled_write_ln(last_trans, false);
            oled_advance_page(true);
            oled_advance_page(true);
            break;
        case DISP_STATUS:
            if (timer_elapsed32(status_time) > STATUS_STAY_TIME) {
                state = DISP_NORMAL;
                status_button_time = 0;
            }
            if (button) {
                status_time = timer_read32();
                if (!button_last) {
                    status_button_time = status_time;
                }
                if (status_button_time && timer_elapsed32(status_button_time) > BUTTON_HOLD_TIME) {
                    restart_advertising_wo_whitelist();
                }
            } else {
                if (button_last && timer_elapsed32(status_button_time) < BUTTON_HOLD_TIME) {
                    device_id ++;
                    if (device_id == 5) {
                        device_id = 0;
                    }
                    restart_advertising_id(device_id);
                }
            }
            uint16_t volt = get_vcc();
            snprintf(buf, 32, "BAT: %umV", volt);
            oled_write_ln(buf, false);
            snprintf(buf, 32, "Device #%u", device_id);
            oled_write_ln(buf, false);
            oled_advance_page(true);
            oled_advance_page(true);
            break;
    }
    button_last = button;
#endif
}
#endif

#endif
