#include <zmk/display/widgets/output_status.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/display/widgets/wpm_status.h>
#include <zmk/display/status_screen.h>
#include <src/lv_widgets/lv_cont.h>

/* #include <logging/log.h> */
/* LOG_MODULE_DECLARE(steno, CONFIG_ZMK_EMBEDDED_STENO_LOG_LEVEL); */

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
