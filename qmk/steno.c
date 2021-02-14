#include <string.h>
#include "steno.h"
#include "keymap_steno.h"
#include "spi.h"
#include "flash.h"
#include <stdio.h>

#ifdef CUSTOM_STENO

#include "hist.h"
#include "lcd.h"
#include "dict_editing.h"
#include <LUFA/Drivers/USB/USB.h>
#include "scsi.h"

#ifdef __AVR__
#ifdef HAS_BATTERY
#include "analog.h"
#endif
#else
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

bool flashing = false;
char last_stroke[24];
char last_trans[128];
uint8_t last_trans_size;

uint8_t hist_ind = 0;
// Index into `history` that marks how far into the past the translation can go; always less than or
// equal to `hist_ind` or 0xFF
uint8_t stroke_start_ind = 0;

#ifndef __AVR__
static bt_state_t bt_state = BT_ACTIVE;
static uint32_t bt_state_time;
#endif
#ifdef STENO_PHONE
static uint8_t locked = 1;
#endif

// Intercept the steno key codes, searches for the stroke, and outputs the output
void _send_steno_chord_user(const uint8_t chord[6]);
bool send_steno_chord_user(const steno_mode_t mode, const uint8_t chord[6]) {
#ifdef CONSOLE_ENABLE
    uint16_t time = timer_read();
#endif
    _send_steno_chord_user(chord);
#ifdef CONSOLE_ENABLE
    steno_debug_ln("time: %ums", timer_elapsed(time));
#endif
    return false;
}

void _send_steno_chord_user(const uint8_t chord[6]) {
#ifndef __AVR__
    bt_state_time = timer_read32();
#endif

    if (flashing) {
        steno_debug_ln("flashing");
        return;
    }
    const uint32_t stroke = qmk_chord_to_stroke(chord);
#ifdef STENO_PHONE
    if (stroke == 0x0CA990) {   //KPROERPG, 2 paws and OE
        locked = !locked;
        return;
    }
    if (locked) {
        return;
    }
#endif

#ifndef __AVR__
    bt_state = BT_ACTIVE;
#endif

    /* #ifdef OLED_DRIVER_ENABLE */
    last_trans_size = 0;
    /* memset(last_trans, 0, 128); */
    stroke_to_string(stroke, last_stroke, NULL);
    /* #endif */
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("Current Editing State: %d", editing_state);
#endif
    if (editing_state == ED_ACTIVE_ADD) {
        if (stroke == 0x008100) {
            editing_state = ED_ACTIVE_ADD_TRANS;
            dicted_add_prompt_trans();
        } else {
            set_stroke(stroke);
#ifdef STENO_DEBUG_DICTED
            steno_debug_ln("entered editing state");
#endif
        }
        return;
    }
    if (editing_state == ED_ACTIVE_ADD_TRANS) {
        if (stroke == 0x008100) {
            editing_state = ED_IDLE;
            dicted_add_done();
            return;
        }
        // `process_output()` will handle the translation and write to buffer
    }

    if (editing_state == ED_ERROR) {
        if (stroke == 0x008100) {
            editing_state = ED_IDLE;
            select_lcd();
            lcd_clear();
            unselect_lcd();
        }

        return;
    }

    if(editing_state == ED_ACTIVE_REMOVE) {
        if (stroke == 0x008100) {
            editing_state = ED_ACTIVE_REMOVE_TRANS;
            dicted_remove_conf_strokes();
        } else {
            set_stroke(stroke);
#ifdef STENO_DEBUG_DICTED
            steno_debug_ln("Added stroke to remove");
#endif
        }
        return;
    }

    if (editing_state == ED_ACTIVE_REMOVE_TRANS) {
        if (stroke == 0x008100) {
            editing_state = ED_IDLE;
            remove_entry();
            select_lcd();
            lcd_clear();
            unselect_lcd();
        }
#ifdef STENO_DEBUG_DICTED
        steno_debug_ln("removed stroke");
#endif
        return;
    }

    if (editing_state == ED_ACTIVE_EDIT_CONF_STROKES) {
        if (stroke == 0x008100) {
            editing_state = ED_ACTIVE_EDIT_TRANS;
            dicted_edit_conf_strokes();
#ifdef STENO_DEBUG_DICTED
            steno_debug_ln("dicted_edit_conf_strokes() executed");
#endif
        } else {
            set_stroke(stroke);
        }
        return;
    }

    if (editing_state == ED_ACTIVE_EDIT_TRANS) {
        if (stroke == 0x008100) {
            editing_state = ED_ACTIVE_EDIT_CONF_TRANS;
            dicted_edit_prompt_trans();
#ifdef STENO_DEBUG_DICTED
            steno_debug_ln("dicted_edit_prompt_trans() executed");
#endif
        }
        return;
    }

    if (editing_state == ED_ACTIVE_EDIT_CONF_TRANS) {
        if (stroke == 0x008100) {
            editing_state = ED_IDLE;
            dicted_edit_done();
#ifdef STENO_DEBUG_DICTED
            steno_debug_ln("edited translation");
#endif
            return;
        }
        // `process_output()` will handle the translation and write to buffer
    }

    if (stroke == 0x1000) { // Asterisk
        hist_ind = HIST_LIMIT(hist_ind - 1);
        hist_undo(hist_ind);
        if (editing_state == ED_IDLE) {
            select_lcd();
            lcd_fill_rect(0, 0, LCD_WIDTH, 64, LCD_WHITE);
            lcd_puts_at(0, 0, "*", 2);
            unselect_lcd();
        }
        return;
    }

    history_t *hist = hist_get(hist_ind);
    hist->stroke = stroke;
    // Default `state` set in last cycle
    search_entry(hist_ind);
    hist->entry = last_entry_ptr;
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("  entry: %06lX", last_entry_ptr);
#endif
    const uint8_t strokes_len = ENTRY_GET_STROKES_LEN(last_entry_ptr);
    if (strokes_len > 1) {
        hist->state = hist_get(HIST_LIMIT(hist_ind - strokes_len + 1))->state;
    }
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("this state[%u]: space: %u, cap: %u, glue: %u", hist_ind, hist->state.space, hist->state.cap, hist->state.glue);
#endif
    state_t new_state = process_output(hist_ind);
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("next state[%u]: space: %u, cap: %u, glue: %u", HIST_LIMIT(hist_ind + 1), new_state.space, new_state.cap, new_state.glue);
#endif
    if (editing_state == ED_IDLE) {
        select_lcd();
        lcd_fill_rect(0, 0, LCD_WIDTH, 64, LCD_WHITE);
        char buf[24];
        if (strokes_len > 0) {
            const uint32_t first_stroke = *((uint32_t *) entry_buf);
            stroke_to_string(first_stroke, buf, NULL);
            lcd_puts_at(0, 0, buf, 2);
            for (uint8_t i = 1; i < strokes_len; i ++) {
                lcd_putc('/', 2);
                const uint32_t stroke = *((uint32_t *) (entry_buf + 3 * i));
                stroke_to_string(stroke, buf, NULL);
                lcd_puts(buf, 2);
            }
            lcd_putc('\n', 2);
            last_trans[last_trans_size] = 0;
            lcd_puts(last_trans, 2);
        } else {
            stroke_to_string(hist->stroke, buf, NULL);
            lcd_puts_at(0, 0, buf, 2);
        }
        unselect_lcd();
    }
    if (hist->len) {
#ifdef STENO_DEBUG_HIST
        steno_debug_ln("hist[%u]:", hist_ind);
        steno_debug_ln("  len: %u, stroke_len: %u", hist->len, ENTRY_GET_STROKES_LEN(hist->entry));
        state_t state = hist->state;
        steno_debug_ln("  space: %u, cap: %u, glue: %u", state.space, state.cap, state.glue);
        char buf[24];
        stroke_to_string(hist->stroke, buf, NULL);
        steno_debug_ln("  stroke: %s", buf);
        if (hist->entry != 0) {
            steno_debug_ln("  entry: %lX", hist->entry & 0xFFFFFF);
        }
#endif
        hist_ind = HIST_LIMIT(hist_ind + 1);
        stroke_start_ind = 0xFF;
    } else {
        stroke_start_ind = hist_ind;
    }
    hist_get(hist_ind)->state = new_state;

#if defined(STENO_DEBUG_HIST) || defined(STENO_DEBUG_FLASH) || defined(STENO_DEBUG_STROKE) || defined(STENO_DEBUG_DICTED)
    steno_debug_ln("--------\n");
#endif
}

// Setup the necessary stuff, init SD card or SPI flash. Delay so that it's easy for `hid-listen` to recognize
// the keyboard
void keyboard_post_init_user(void) {
    hist_get(0)->state.cap = CAPS_CAP;
#ifdef USE_SPI_FLASH
    spi_init();
    flash_init();
    lcd_init();
#else
    extern FATFS fat_fs;
    if (pf_mount(&fat_fs)) {
        steno_error_ln("Volume");
        goto error;
    }

    FRESULT res = pf_open("STENO.BIN");
    if (res) {
        steno_error_ln("File: %X", res);
        goto error;
    }

    steno_error_ln("init");
    return;
error:
    steno_error_ln("Can't init");
    while(1);
#endif
    
#ifndef __AVR__
    nrf_gpio_cfg_input(BUTTON, NRF_GPIO_PIN_PULLUP);
    bt_state_time = timer_read32();
#endif
}

void matrix_init_user() {
#ifndef __AVR__
    set_usb_enabled(true);
    set_ble_enabled(false);
#endif
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

#ifndef __AVR__
    if (bt_state != BT_ACTIVE) {
        nrf_pwr_mgmt_run();
    } else {
        if (timer_elapsed32(bt_state_time) > BT_ACTIVE_HOLD_TIME) {
            steno_debug_ln("Going into idle");
            bt_state = BT_IDLE;
        }
    }
#endif

    //oled_set_contrast(0);
    char buf[32];
#ifdef __AVR__
#ifdef HAS_BATTERY
    char buf[32];
    uint16_t adc = analogReadPin(B5);
    uint16_t volt = (uint32_t) adc * 33 * 2 * 10 / 1024;
    sprintf(buf, "BAT: %u.%uV", volt / 100, volt % 100);
    oled_write_ln(buf, false);
#endif
    oled_write_ln(last_stroke, false);
    oled_write_ln(last_trans, false);
#else
    char buf[32];
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
            uint8_t pers = 0;
            if (volt < 3300) {
                pers = 0;
            } else if (volt < 3600) {
                pers = (volt - 3300) / 30;
            } else {
                pers = 10 + (volt - 3600) * 3 / 20;
            }
            snprintf(buf, 32, "BAT: %u%% (%umV)", pers, volt);
            oled_write_ln(buf, false);
            snprintf(buf, 32, "Device #%u", device_id);
            oled_write_ln(buf, false);
#ifdef STENO_PHONE
            if (locked) {
                oled_write_ln("Locked", false);
            }
#else
            oled_advance_page(true);
#endif
            oled_advance_page(true);
            break;
    }
    button_last = button;
#endif
}
#endif

#endif
