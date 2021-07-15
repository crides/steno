#include <zmk/display/widgets/output_status.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/display/widgets/wpm_status.h>
#include <zmk/display/status_screen.h>
#include <src/lv_widgets/lv_cont.h>

#include "steno.h"
#include "stroke.h"

static struct zmk_widget_battery_status battery_status_widget;
static struct zmk_widget_output_status output_status_widget;

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)
static struct zmk_widget_layer_status layer_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_WPM_STATUS)
static struct zmk_widget_wpm_status wpm_status_widget;
#endif

typedef struct {
    lv_obj_t *status_cont;
    lv_obj_t *tape_cont;

    lv_obj_t *tape;
    lv_obj_t *last_entry;
} steno_screen_t;

static steno_screen_t steno_screen;

lv_style_t style, tape_style;

LV_FONT_DECLARE(iosevka_8);
LV_FONT_DECLARE(iosevka_16);

#define TAPE_LINES ((CONFIG_LVGL_VER_RES_MAX / 16) - 4)

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen;

    lv_style_init(&style);
    lv_style_set_bg_color(&style, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    screen = lv_obj_create(NULL, NULL);
    lv_obj_add_style(screen, LV_LABEL_PART_MAIN, &style);

    steno_screen.status_cont = lv_cont_create(screen, NULL);
    lv_obj_align(steno_screen.status_cont, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_cont_set_fit2(steno_screen.status_cont, LV_FIT_MAX, LV_FIT_TIGHT);
    lv_cont_set_layout(steno_screen.status_cont, LV_LAYOUT_OFF);

    zmk_widget_output_status_init(&output_status_widget, steno_screen.status_cont);
    lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

    zmk_widget_battery_status_init(&battery_status_widget, steno_screen.status_cont);
    lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), NULL, LV_ALIGN_IN_TOP_LEFT, CONFIG_LVGL_HOR_RES_MAX / 4, 0);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)
    zmk_widget_layer_status_init(&layer_status_widget, steno_screen.status_cont);
    lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), NULL, LV_ALIGN_IN_TOP_LEFT, CONFIG_LVGL_HOR_RES_MAX / 4 * 2, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_WPM_STATUS)
    zmk_widget_wpm_status_init(&wpm_status_widget, steno_screen.status_cont);
    lv_obj_align(zmk_widget_wpm_status_obj(&wpm_status_widget), NULL, LV_ALIGN_IN_TOP_LEFT, CONFIG_LVGL_HOR_RES_MAX / 4 * 3, 0);
#endif

    steno_screen.tape_cont = lv_obj_create(screen, NULL);
    lv_style_init(&tape_style);
    lv_style_set_text_font(&tape_style, LV_STATE_DEFAULT, &iosevka_16);
    lv_style_set_bg_color(&tape_style, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_obj_add_style(steno_screen.tape_cont, LV_CONT_PART_MAIN, &tape_style);
    lv_obj_align(steno_screen.tape_cont, steno_screen.status_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    /* lv_cont_set_fit2(steno_screen.tape_cont, LV_FIT_MAX, LV_FIT_PARENT); */
    lv_obj_set_width(steno_screen.tape_cont, CONFIG_LVGL_HOR_RES_MAX);
    lv_obj_set_height(steno_screen.tape_cont, CONFIG_LVGL_VER_RES_MAX - lv_obj_get_height(steno_screen.status_cont));

    steno_screen.tape = lv_label_create(steno_screen.tape_cont, NULL);
    lv_label_set_text(steno_screen.tape, "STOIN");
    lv_obj_align(steno_screen.tape, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_height(steno_screen.tape, TAPE_LINES * 16);

    steno_screen.last_entry = lv_label_create(steno_screen.tape_cont, NULL);
    lv_label_set_text(steno_screen.last_entry, "\n");
    lv_obj_align(steno_screen.last_entry, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);  // TODO alignment offset might be problematic

    lv_obj_t *sep = lv_label_create(steno_screen.tape_cont, NULL);
    lv_label_set_text(sep, "------------------------------");
    lv_obj_align(sep, steno_screen.last_entry, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
    lv_obj_set_width(sep, CONFIG_LVGL_HOR_RES_MAX);

    return screen;
}

void disp_init(void) {
}

static uint32_t tape_strokes[TAPE_LINES];
static void tape_show(const uint32_t stroke, const uint8_t lines) {
    steno_debug_ln(DWF("06") ", %u", stroke, lines);
    for (uint8_t i = 0; i < (lines - 1); i ++) {
        tape_strokes[i] = tape_strokes[i + 1];
    }
    tape_strokes[lines - 1] = stroke;
    char tape[TAPE_LINES * 32] = { [0] = 0 };
    for (uint8_t i = 0; i < lines; i ++) {
        char buf[32];
        stroke_to_tape(tape_strokes[i], buf);
        strcat(tape, buf);
        strcat(tape, "\n");
    }
    lv_label_set_text(steno_screen.tape, tape);
}

void disp_tape_show_raw_stroke(const uint32_t stroke) {
    tape_show(stroke, TAPE_LINES);
    char buf[32];
    stroke_to_string(stroke, buf, NULL);
    strcat(buf, "\n");
    lv_label_set_text(steno_screen.last_entry, buf);
}

void disp_tape_show_star(void) {
    disp_tape_show_raw_stroke(STENO_STAR);
}

static const uint8_t *strokes;
static uint8_t strokes_len;

void disp_tape_show_strokes(const uint8_t *_strokes, const uint8_t _len) {
    strokes = _strokes;
    strokes_len = _len;
}

void disp_tape_show_trans(const char *const trans) {
    char entry[8 * 32] = { [0] = 0 };
    uint32_t last_stroke = 0;
    for (uint8_t i = 0; i < strokes_len; i ++) {
        const uint32_t stroke = *((uint32_t *) &strokes[i * STROKE_SIZE]) & 0xFFFFFF;
        char buf[32];
        stroke_to_string(stroke, buf, NULL);
        strcat(entry, buf);
        if (i == strokes_len - 1) {
            last_stroke = stroke;
        } else {
            strcat(entry, "/");
        }
    }
    strcat(entry, "\n");
    strcat(entry, trans);
    lv_label_set_text(steno_screen.last_entry, entry);
    const uint8_t entry_lines = lv_obj_get_height(steno_screen.last_entry) / 16;
    tape_show(last_stroke, TAPE_LINES - (entry_lines - 2));
}

#ifndef STENO_READONLY

static const char *ED_ENTER_STROKES_MODE = "DICT UPDATE\nEntering outline";
static const char *ED_ENTER_TRANS_MODE = "DICT UPDATE\nEntering translation";
static const char *ED_DONE = "DICT UPDATE\nEntry added";
static const char *ED_ABORT = "DICT UPDATE\nCancelled";
static const char *ED_NOSTORAGE = "DICT UPDATE\nNo storage";
static const char *ED_REMOVED = "DICT UPDATE\nEntry removed";

static const char *ED_ENTER_STROKES_MSG =
    "Enter outline,\n"
    "or leave empty to cancel\n\n"
    "   *  Delete Stroke\n"
    " R-R  Confirm\n";
static const char *ED_ENTER_TRANS_MSG =
    "Enter translation,\n"
    "or leave empty to\n"
    "remove entry\n\n"
    "   *  Undo\n"
    " R-R  Confirm\n";
static const char *ED_ENTER_TRANS_DURING_MSG = "Enter translation:\n\n";
static const char *ED_CONF_ENTRY_START = "Entry already exists:\n\n";
static const char *ED_CONF_ENTRY_END =
    "\n\n"
    " R-R  Overwrite\n"
    "   *  Cancel\n";
static const char *ED_ABORT_MSG =
    "Cancelled\n\n"
    " R-R  Return";
static const char *ED_NOSTORAGE_MSG =
    "Can't update dictionary:\n"
    "No more storage\n\n"
    " R-R  Return";
static const char *ED_REMOVED_MSG =
    "Entry removed\n\n"
    " R-R  Return";

static uint32_t tape_before_edit[TAPE_LINES];
static uint32_t trans_len;

static void save_tape(void) {
    for (int i = 0; i < TAPE_LINES; i++) {
        tape_before_edit[i] = tape_strokes[i];
        tape_strokes[i] = 0;
    }
}

static void rewind_tape() {
    for (int i = TAPE_LINES - 1; i >= 1; i--) {
        tape_strokes[i] = tape_strokes[i - 1];
    }
    tape_strokes[0] = 0;
}

static void restore_tape(void) {
    for (int i = 0; i < TAPE_LINES; i++) {
        tape_strokes[i] = tape_before_edit[i];
        tape_before_edit[i] = 0;
    }
}

static void clear_tape(void) {
    for (int i = 0; i < TAPE_LINES; i++) {
        tape_strokes[i] = 0;
    }
}

static void tape_show_full(void) {
    char tape[TAPE_LINES * 32] = {[0] = 0};
    for (uint8_t i = 0; i < TAPE_LINES; i++) {
        char buf[32];
        stroke_to_tape(tape_strokes[i], buf);
        strcat(tape, buf);
        strcat(tape, "\n");
    }
    lv_label_set_text(steno_screen.tape, tape);
}

void disp_prompt_strokes(void) {
    save_tape();
    lv_label_set_text(steno_screen.last_entry, ED_ENTER_STROKES_MODE);
    lv_label_set_text(steno_screen.tape, ED_ENTER_STROKES_MSG);
}

void disp_stroke_edit_add(const uint32_t stroke, const uint8_t num_strokes) {
    tape_show(stroke, TAPE_LINES);
}

void disp_stroke_edit_remove(const uint32_t last, const uint8_t num_strokes) {
    if (num_strokes == 0) {
        clear_tape();
        lv_label_set_text(steno_screen.tape, ED_ENTER_STROKES_MSG);
    } else {
        rewind_tape();
        tape_show_full();
    }
}

void disp_conf_entry(const char *const s) {
    char output[TAPE_LINES * 32] = {[0] = 0};
    strcat(output, ED_CONF_ENTRY_START);
    strcat(output, s);
    strcat(output, ED_CONF_ENTRY_END);

    lv_label_set_text(steno_screen.last_entry, ED_ENTER_STROKES_MODE);
    lv_label_set_text(steno_screen.tape, output);
}

void disp_prompt_trans(void) {
    lv_label_set_text(steno_screen.last_entry, ED_ENTER_TRANS_MODE);
    lv_label_set_text(steno_screen.tape, ED_ENTER_TRANS_MSG);
    trans_len = 0;
}

void disp_trans_edit_back(const uint8_t len) {
    if (len == trans_len) {
        lv_label_set_text(steno_screen.tape, ED_ENTER_TRANS_MSG);
    } else {
        char tape[TAPE_LINES * 32] = {[0] = 0};
        strcat(tape, lv_label_get_text(steno_screen.tape));

        int tape_len = strlen(tape);
        tape[tape_len - len] = 0;
        lv_label_set_text(steno_screen.tape, tape);
    }
    trans_len -= len;
}

void disp_trans_edit_handle_char(const char c) {
    char tape[TAPE_LINES * 32] = {[0] = 0};
    if (trans_len == 0) {
        strcat(tape, ED_ENTER_TRANS_DURING_MSG);
    } else {
        strcat(tape, lv_label_get_text(steno_screen.tape));
    }

    int tape_len = strlen(tape);
    tape[tape_len++] = c;
    tape[tape_len] = 0;
    trans_len++;
    lv_label_set_text(steno_screen.tape, tape);
}

void disp_trans_edit_handle_str(const char *const s) {
    char tape[TAPE_LINES * 32] = {[0] = 0};
    if (trans_len == 0) {
        strcat(tape, ED_ENTER_TRANS_DURING_MSG);
    } else {
        strcat(tape, lv_label_get_text(steno_screen.tape));
    }

    strcat(tape, s);
    trans_len += strlen(s);
    lv_label_set_text(steno_screen.tape, tape);
}

void disp_dicted_done(void) {
    restore_tape();
    tape_show_full();
    lv_label_set_text(steno_screen.last_entry, ED_DONE);
}

void disp_show_abort(void) {
    restore_tape();
    lv_label_set_text(steno_screen.tape, ED_ABORT_MSG);
    lv_label_set_text(steno_screen.last_entry, ED_ABORT);
}

void disp_show_entry_collision(void) {
#if STENO_DEBUG
    steno_debug_ln("disp_show_entry_collision");
#endif
}

void disp_show_noentry(void) {
#if STENO_DEBUG
    steno_debug_ln("disp_show_noentry");
#endif
}

void disp_show_nostorage(void) {
    restore_tape();
    lv_label_set_text(steno_screen.tape, ED_NOSTORAGE_MSG);
    lv_label_set_text(steno_screen.last_entry, ED_NOSTORAGE);
}

void disp_show_removed(void) {
    restore_tape();
    lv_label_set_text(steno_screen.tape, ED_REMOVED_MSG);
    lv_label_set_text(steno_screen.last_entry, ED_REMOVED);
}

void disp_unshow_error(void) {
    tape_show_full();
    lv_label_set_text(steno_screen.last_entry, "");
}

#endif /* STENO_READONLY */
