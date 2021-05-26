#include <stdio.h>
#include <string.h>
#include "steno.h"
#include "store.h"
#include "hist.h"
#ifndef STENO_READONLY
#include "dict_editing.h"
#endif
#ifndef STENO_NOUI
#include "disp.h"
#endif
#ifdef STENO_FLASH_LOGGING
#include "flog.h"
#endif

bool flashing = false;
char last_trans[128];
uint8_t last_trans_size;

uint8_t hist_ind = 0;
// Index into `history` that marks how far into the past the translation can go; always less than or
// equal to `hist_ind` or 0xFF
uint8_t stroke_start_ind = 0;
uint16_t time = 0;

#define print_time(sec) steno_debug_ln("<> " sec ": %ums", timer_elapsed(time));

// Intercept the steno key codes, searches for the stroke, and outputs the output
void _ebd_steno_process_stroke(const uint32_t stroke);
void ebd_steno_process_stroke(const uint32_t stroke) {
#ifdef STENO_DEBUG
    time = timer_read();
#endif
    _ebd_steno_process_stroke(stroke);
#ifdef STENO_FLASH_LOGGING
    flog_finish_cycle();
#endif
}

void _ebd_steno_process_stroke(const uint32_t stroke) {
    if (flashing) {
        steno_debug_ln("flashing");
        return;
    }

    last_trans_size = 0;

#ifndef STENO_READONLY
    if (handle_dict_editing(stroke)) {
        return;
    }
#endif
    print_time("dicted");

    if (stroke == STENO_STAR) {
        hist_ind = HIST_LIMIT(hist_ind - 1);
        hist_undo(hist_ind);
        print_time("undo");
#ifndef STENO_NOUI
#ifndef STENO_READONLY
        if (editing_state == ED_IDLE)
#endif
        {
            disp_tape_show_star();
        }
#endif
        return;
    }

    history_t *hist = hist_get(hist_ind);
    hist->stroke = stroke;
    // Default `state` set in last cycle
    print_time("start search");
    const uint32_t bucket = search_entry(hist_ind);
    print_time("search");
    hist->bucket = bucket;
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("  bucket: %08lX", bucket);
#endif
    const uint8_t strokes_len = BUCKET_GET_STROKES_LEN(bucket);
    if (strokes_len > 1) {
        hist->state = hist_get(HIST_LIMIT(hist_ind - strokes_len + 1))->state;
    }
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("this %u: scg: %u%u%u", hist_ind, hist->state.space, hist->state.cap, hist->state.glue);
#endif
    const state_t new_state = process_output(hist_ind);
    print_time("out");
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("next %u: scg: %u%u%u", HIST_LIMIT(hist_ind + 1), new_state.space, new_state.cap, new_state.glue);
#endif
#ifndef STENO_NOUI
#ifndef STENO_READONLY
    if (editing_state == ED_IDLE)
#endif
    {
        if (strokes_len > 0) {
            disp_tape_show_strokes(kvpair_buf, strokes_len);
            last_trans[last_trans_size] = 0;
            disp_tape_show_trans(last_trans);
        } else {
            disp_tape_show_raw_stroke(hist->stroke);
        }
    }
#endif
    if (hist->len) {
#ifdef STENO_DEBUG_HIST
        steno_debug_ln("hist %u:", hist_ind);
        steno_debug_ln("  len: %u, stroke: %u"
#ifndef STENO_NOORTHOGRAPHY
                ", ortho: %u"
#endif
                , hist->len, BUCKET_GET_STROKES_LEN(hist->bucket)
#ifndef STENO_NOORTHOGRAPHY
                , hist->ortho_len
#endif
                );
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

#ifdef STENO_DEBUG
    print_time("final");
    steno_debug_ln("----\n");
#endif
}

// Setup the necessary stuff, init SPI flash
void ebd_steno_init(void) {     // to avoid clashing with `steno_init` in QMK
    hist_get(0)->state.cap = CAPS_CAP;
    store_init();
#ifndef STENO_NOUI
    disp_init();
#endif
#ifdef STENO_FLASH_LOGGING
    flog_init();
#endif
}
