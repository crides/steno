#include <string.h>
#include "steno.h"
#include "store.h"
#include <stdio.h>
#include "hist.h"
#include "dict_editing.h"
#include "stroke.h"
#ifndef STENO_NOUI
#include "disp.h"
#endif

editing_state_t editing_state = ED_IDLE;
// Text buffer for entering translation
uint8_t entry_buf[128];
uint8_t entry_buf_len = 0;
static uint8_t strokes[MAX_STROKE_NUM * STROKE_SIZE];
static uint8_t strokes_len = 0;

void dicted_update(void) {
    editing_state = ED_ENTER_STROKES;
    disp_prompt_strokes();
    strokes_len = 0;
}

static void dicted_add_stroke(const uint32_t stroke) {
    if (stroke == STENO_STAR) {
        if (strokes_len != 0) {
            strokes_len --;
            const uint32_t temp_last_stroke = STROKE_FROM_PTR(&strokes[STROKE_SIZE * strokes_len]);
            disp_stroke_edit_remove(temp_last_stroke, strokes_len);
        }
    } else if (strokes_len < MAX_STROKE_NUM) {
        disp_stroke_edit_add(stroke, strokes_len);
        strokes[STROKE_SIZE * strokes_len + 2] = (stroke >> 16) & 0xFF;
        strokes[STROKE_SIZE * strokes_len + 1] = (stroke >> 8) & 0xFF;
        strokes[STROKE_SIZE * strokes_len] = stroke & 0xFF;
        strokes_len ++;
    }
}

static void dicted_prompt_trans(void) {
    disp_prompt_trans();
    extern uint8_t hist_ind;
    hist_get(hist_ind)->state.space = 0;
    entry_buf_len = 0;
}

static bool add_entry(void) {
#ifdef STENO_DEBUG_FLASH
    flash_debug_enable = 1;
#endif
    uint8_t bloq;
    const uint8_t entry_len = strokes_len * STROKE_SIZE + 1 + entry_buf_len;
    if (entry_len <= 16) {
        bloq = 0;
    } else if (entry_len <= 32) {
        bloq = 1;
    } else if (entry_len <= 64) {
        bloq = 2;
    } else {
        bloq = 3;
    }
    const uint32_t block_ind = freemap_req(bloq);
    if (block_ind == -1) {
        disp_show_nostorage();
        editing_state = ED_ERROR;
        return true;
    }
    const uint32_t block_addr = block_ind * 16 + KVPAIR_BLOCK_START;
    const attr_t attr = { .space_prev = 1, .space_after = 1, .glue = 0 };
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("blok addr %06lX", block_addr);
#endif
    store_write_direct(block_addr, (const uint8_t *const) strokes, strokes_len * STROKE_SIZE);
    store_write_direct(block_addr + strokes_len * STROKE_SIZE, (const uint8_t *const) &attr, 1);
    store_write_direct(block_addr + strokes_len * STROKE_SIZE + 1, entry_buf, entry_buf_len);
    const uint32_t bucket_addr = find_strokes((uint8_t *) strokes, strokes_len, 1);
    const uint32_t bucket = (uint32_t) entry_buf_len << 24 | ((block_addr - KVPAIR_BLOCK_START) & 0xFFFFF0) | (strokes_len & 0x0F);
    store_write_direct(bucket_addr, (const uint8_t *const) &bucket, BUCKET_SIZE);
    store_flush();
#ifdef STENO_DEBUG_FLASH
    flash_debug_enable = 0;
#endif
    return false;
}

static void remove_entry(const uint32_t bucket) {
#ifdef STENO_DEBUG_FLASH
    flash_debug_enable = 1;
#endif
    const uint32_t last_entry_addr = BUCKET_GET_ADDR(bucket);
    const uint8_t kvpair_len = BUCKET_GET_ENTRY_LEN(bucket) + 1 + BUCKET_GET_STROKES_LEN(bucket) * STROKE_SIZE;
    store_erase_partial(last_entry_addr, kvpair_len);
    // TODO Erase bucket and relocate all the later buckets
    // TODO Free block in allocator

#ifdef STENO_DEBUG_FLASH
    flash_debug_enable = 0;
#endif
}

static uint32_t dicted_conf_entry(void) {
    if (strokes_len == 0) {
        editing_state = ED_ERROR;
        disp_show_abort();
        return 0;
    }

#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("dict conf entry");
    print_strokes(strokes, strokes_len);
#endif

    const uint32_t bucket = find_strokes((uint8_t *) strokes, strokes_len, 0);
    if (bucket == 0 || bucket == 0xFFFFFFFF) {
        editing_state = ED_ENTER_TRANS;
        dicted_prompt_trans();
        return 0;
    }
    read_entry(bucket, kvpair_buf);
    const uint8_t entry_len = BUCKET_GET_ENTRY_LEN(bucket);
    const uint8_t stroke_byte_len = STROKE_SIZE * strokes_len;
    char entry_trans[entry_len + 1];

    memcpy(entry_trans, kvpair_buf + stroke_byte_len + 1, entry_len);
    entry_trans[entry_len] = 0;
    disp_conf_entry(entry_trans);
    return bucket;
}

static void dicted_enter_trans(const bool edit) {
    if (entry_buf_len == 0) {
        if (edit) {
            disp_show_removed();
        } else {
            disp_show_abort();
        }
        editing_state = ED_ERROR;
        return;
    }
    if (add_entry()) {
        return;
    }
}

bool _handle_dict_editing(const uint32_t stroke) {
    static uint32_t remove_bucket = 0;
    static bool edit = false;
    switch (editing_state) {
    case ED_ENTER_STROKES:
        if (stroke == STENO_R_R) {
            editing_state = ED_CONF_EDIT;
            remove_bucket = dicted_conf_entry();
            edit = remove_bucket != 0;
        } else {
            dicted_add_stroke(stroke);
        }
        return true;

    case ED_CONF_EDIT:
        switch (stroke) {
        case STENO_R_R:
            editing_state = ED_ENTER_TRANS;
            remove_entry(remove_bucket);
            dicted_prompt_trans();
            return true;
        case STENO_STAR:
            editing_state = ED_ERROR;
            disp_show_abort();
            return true;
        }
        return true;

    case ED_ENTER_TRANS:
        if (stroke == STENO_R_R) {
            editing_state = ED_IDLE;
            dicted_enter_trans(edit);
            if (editing_state != ED_ERROR) {
                disp_dicted_done();
            }
            return true;
        }
        return false;

    case ED_ERROR:
        if (stroke == STENO_R_R) {
            editing_state = ED_IDLE;
            disp_unshow_error();
        }
        return true;

    case ED_IDLE:
        return false;
    // There should be no other cases
    }
    return false;
}

bool handle_dict_editing(const uint32_t stroke) {
#ifdef STENO_DEBUG_DICTED
    const uint8_t old_state = editing_state;
#endif
    const bool ret = _handle_dict_editing(stroke);
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("dicted() -> %b, state: %u -> %u", ret, old_state, editing_state);
#endif
    return ret;
}
