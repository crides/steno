#include <string.h>

#include "stroke.h"
#include "impl/qmk/lcd.h"
#include "impl/qmk/spi.h"

static void disp_back(const uint8_t len) {
    select_lcd();
    // TODO optimize?
    for (uint8_t i = 0; i < len; i++) {
        lcd_back(2);
    }
    unselect_lcd();
}

static void disp_puts(const char *const s) {
    select_lcd();
    lcd_puts(s, 2);
    unselect_lcd();
}

static void disp_puts_at(const int16_t x, const int16_t y, const char *const s) {
    select_lcd();
    lcd_puts_at(x, y, s, 2);
    unselect_lcd();
}

static void disp_putc(const char c) {
    select_lcd();
    lcd_putc(c, 2);
    unselect_lcd();
}

static void disp_clear(void) {
    select_lcd();
    lcd_clear_dirty();
    unselect_lcd();
}

static void disp_tape_show_stroke(const uint32_t stroke) {
    char buf[24];
    stroke_to_string(stroke, buf, NULL);
    disp_puts(buf);
}

#ifdef STENO_STROKE_DISPLAY
static void disp_stenotype_show_stroke(const uint32_t stroke) {
    select_lcd();
    lcd_stenotype_update(stroke);
    unselect_lcd();
}
#endif

// NOTE: Though some displays may use SPI, we only assume that it's initialized in the flash init section, cuz that always uses SPI
void disp_init(void) {
    lcd_init();
}

void disp_tape_show_star(void) {
    disp_clear();
    disp_puts_at(0, 0, "*");
#ifdef STENO_STROKE_DISPLAY
    disp_stenotype_show_stroke(STENO_STAR);
#endif
}

void disp_tape_show_raw_stroke(const uint32_t stroke) {
    disp_clear();
    select_lcd();
    lcd_pos(0, 0);
    unselect_lcd();
    disp_tape_show_stroke(stroke);
#ifdef STENO_STROKE_DISPLAY
    disp_stenotype_show_stroke(stroke);
#endif
}

void disp_tape_show_strokes(const uint8_t *strokes, const uint8_t len) {
    disp_clear();
    const uint32_t first_stroke = *((uint32_t *) kvpair_buf);
    select_lcd();
    lcd_pos(0, 0);
    unselect_lcd();
    disp_tape_show_stroke(first_stroke);
    for (uint8_t i = 1; i < len; i ++) {
        disp_putc('/');
        const uint32_t stroke = *((uint32_t *) (kvpair_buf + STROKE_SIZE * i));
        disp_tape_show_stroke(stroke);
#ifdef STENO_STROKE_DISPLAY
        if (i == len - 1) {
            disp_stenotype_show_stroke(stroke);
        }
#endif
    }
#ifdef STENO_STROKE_DISPLAY
    if (len == 1) {
        disp_stenotype_show_stroke(first_stroke);
    }
#endif
    disp_putc('\n');
}

void disp_tape_show_trans(const char *const trans) {
    disp_puts(trans);
}

void disp_trans_edit_handle_str(const char *const s) {
    disp_puts(s);
}

void disp_trans_edit_handle_char(const char c) {
    disp_putc(c);
}

void disp_trans_edit_back(const uint8_t len) {
    disp_back(len);
}

void disp_prompt_strokes(void) {
    disp_clear();
    disp_puts_at(0, 0, "Strokes:\n");
}

void disp_prompt_trans(void) {
    disp_puts("\nEnter translation:\n");
}

void disp_dicted_done(void) {
    disp_clear();
}

void disp_conf_entry(const char *const s) {
    disp_puts("\nEntry to edit:\n");
    disp_puts(s);
    disp_puts("\nTo edit, press enter (R-R)");
}

void disp_show_abort(void) {
    disp_puts("\nAborted");
}

void disp_show_noentry(void) {
    disp_puts("\nNo Entry");
}

void disp_show_removed(void) {
    disp_puts("\nRemoved");
}

void disp_unshow_error(void) {
    disp_clear();
}

void disp_show_entry_collision(void) {
    disp_puts("\nEntry already exists");
}

void disp_show_nostorage(void) {
    disp_puts("\nNo storage");
}

void disp_stroke_edit_add(const uint32_t stroke, const uint8_t num_strokes) {
    char buf[24];
    stroke_to_string(stroke, buf, NULL);
    if (num_strokes > 0) {
        disp_putc('/');
    }
    disp_puts(buf);
}

void disp_stroke_edit_remove(const uint32_t last, const uint8_t num_strokes) {
    char buf[24];
    uint8_t len;
    stroke_to_string(last, buf, &len);
    disp_back(len);
}
