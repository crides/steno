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

bool flashing = false;
char last_stroke[24];
char last_trans[128];
uint8_t last_trans_size;

uint8_t hist_ind = 0;
// Index into `history` that marks how far into the past the translation can go; always less than or
// equal to `hist_ind` or 0xFF
uint8_t stroke_start_ind = 0;

// Intercept the steno key codes, searches for the stroke, and outputs the output
void _send_steno_chord_user(const uint8_t chord[6]);
bool send_steno_chord_user(const steno_mode_t mode, const uint8_t chord[6]) {
#ifdef CONSOLE_ENABLE
    const uint16_t time = timer_read();
#endif
    _send_steno_chord_user(chord);
#ifdef CONSOLE_ENABLE
    steno_debug_ln("time: %ums", timer_elapsed(time));
#endif
    return false;
}

void _send_steno_chord_user(const uint8_t chord[6]) {
    if (flashing) {
        steno_debug_ln("flashing");
        return;
    }
    const uint32_t stroke = qmk_chord_to_stroke(chord);

    /* #ifdef OLED_DRIVER_ENABLE */
    last_trans_size = 0;
    /* memset(last_trans, 0, 128); */
    stroke_to_string(stroke, last_stroke, NULL);
    /* #endif */
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("Current Editing State: %d", editing_state);
#endif
    if (editing_state == ED_ACTIVE_ADD) {
        if (stroke == STENO_R_R) {
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
        if (stroke == STENO_R_R) {
            editing_state = ED_IDLE;
            dicted_add_done();
            return;
        }
        // `process_output()` will handle the translation and write to buffer
    }

    if (editing_state == ED_ERROR) {
        if (stroke == STENO_R_R) {
            editing_state = ED_IDLE;
            select_lcd();
            lcd_clear();
            unselect_lcd();
        }
        return;
    }

    if (editing_state == ED_ACTIVE_REMOVE) {
        if (stroke == STENO_R_R) {
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
        if (stroke == STENO_R_R) {
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
        if (stroke == STENO_R_R) {
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
        if (stroke == STENO_R_R) {
            editing_state = ED_ACTIVE_EDIT_CONF_TRANS;
            dicted_edit_prompt_trans();
#ifdef STENO_DEBUG_DICTED
            steno_debug_ln("dicted_edit_prompt_trans() executed");
#endif
        }
        return;
    }

    if (editing_state == ED_ACTIVE_EDIT_CONF_TRANS) {
        if (stroke == STENO_R_R) {
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
    hist->bucket = last_bucket;
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("  bucket: %08lX", last_bucket);
#endif
    const uint8_t strokes_len = BUCKET_GET_STROKES_LEN(last_bucket);
    if (strokes_len > 1) {
        hist->state = hist_get(HIST_LIMIT(hist_ind - strokes_len + 1))->state;
    }
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("this %u: scg: %u%u%u", hist_ind, hist->state.space, hist->state.cap, hist->state.glue);
#endif
    const state_t new_state = process_output(hist_ind);
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("next %u: scg: %u%u%u", HIST_LIMIT(hist_ind + 1), new_state.space, new_state.cap, new_state.glue);
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
                const uint32_t stroke = *((uint32_t *) (entry_buf + STROKE_SIZE * i));
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
        steno_debug_ln("hist %u:", hist_ind);
        steno_debug_ln("  len: %u, stroke_len: %u", hist->len, BUCKET_GET_STROKES_LEN(hist->bucket));
        const state_t state = hist->state;
        steno_debug_ln("  scg: %u%u%u", state.space, state.cap, state.glue);
        char buf[24];
        stroke_to_string(hist->stroke, buf, NULL);
        steno_debug_ln("  stroke: %s", buf);
        if (hist->bucket != 0) {
            steno_debug_ln("  bucket: %08lX", hist->bucket);
        }
#endif
        hist_ind = HIST_LIMIT(hist_ind + 1);
        stroke_start_ind = 0xFF;
    } else {
        stroke_start_ind = hist_ind;
    }
    hist_get(hist_ind)->state = new_state;

#if defined(STENO_DEBUG_HIST) || defined(STENO_DEBUG_FLASH) || defined(STENO_DEBUG_STROKE) || defined(STENO_DEBUG_DICTED)
    steno_debug_ln("----\n");
#endif
}

// Setup the necessary stuff, init SPI flash
void keyboard_post_init_user(void) {
    hist_get(0)->state.cap = CAPS_CAP;
    spi_init();
    flash_init();
    lcd_init();
}

void matrix_init_user() {
    steno_set_mode(STENO_MODE_GEMINI);
}

#ifdef OLED_DRIVER_ENABLE
void oled_task_user(void) {
    oled_write_ln(last_stroke, false);
    oled_write_ln(last_trans, false);
}
#endif

#endif
