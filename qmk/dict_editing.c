#include <string.h>
#include "steno.h"
#include "keymap_steno.h"
#include "store.h"
#include <stdio.h>
#include "hist.h"
#include "dict_editing.h"
#include "stroke.h"
#include "disp.h"

editing_state_t editing_state = ED_IDLE;
// Buffer for a page; used for unparsed/formatted entry and copying between pages
uint8_t entry_buf[128];
uint8_t entry_buf_len = 0;
static uint8_t curr_stroke[MAX_STROKE_NUM * STROKE_SIZE];
static uint8_t curr_stroke_size = 0;
static uint8_t entry_length = 0;
static uint32_t remove_bucket = 0;

void dicted_add_prompt_strokes(void) {
    editing_state = ED_ACTIVE_ADD;
    disp_prompt_add_stroke();
    curr_stroke_size = 0;
}

static void dicted_add_stroke(const uint32_t stroke) {
    if (stroke == 0x1000) {
        if (curr_stroke_size != 0) {
            curr_stroke_size --;
            const uint32_t temp_last_stroke = STROKE_FROM_PTR(&curr_stroke[STROKE_SIZE * curr_stroke_size]);
            disp_stroke_edit_remove(temp_last_stroke, curr_stroke_size);
        }
    } else if (curr_stroke_size < MAX_STROKE_NUM) {
        disp_stroke_edit_add(stroke, curr_stroke_size);
        curr_stroke[STROKE_SIZE * curr_stroke_size + 2] = (stroke >> 16) & 0xFF;
        curr_stroke[STROKE_SIZE * curr_stroke_size + 1] = (stroke >> 8) & 0xFF;
        curr_stroke[STROKE_SIZE * curr_stroke_size] = stroke & 0xFF;
        curr_stroke_size ++;
    }
}

static void dicted_add_prompt_trans(void) {
    if (curr_stroke_size == 0) {
        editing_state = ED_ERROR;
        disp_show_abort();
        return;
    }

    const uint32_t bucket = find_strokes((uint8_t *) curr_stroke, curr_stroke_size, 0);
    const uint8_t stroke_len = BUCKET_GET_STROKES_LEN(bucket);
    if (stroke_len != 0 && stroke_len != 0xF) {
        editing_state = ED_ERROR;
        disp_show_entry_collision();
        return;
    }
    disp_prompt_trans();
    extern uint8_t hist_ind;
    hist_get(hist_ind)->state.space = 0;
    entry_buf_len = 0;
}

static bool add_entry(void) {
#ifdef STENO_DEBUG_FLASH
    flash_debug_enable = 1;
#endif
    if (entry_buf_len == 0) {
        disp_show_abort();
        editing_state = ED_ERROR;
        return true;
    }

    uint8_t bloq;
    const uint8_t entry_len = curr_stroke_size * STROKE_SIZE + 1 + entry_buf_len;
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
    store_write_direct(block_addr, (const uint8_t *const) curr_stroke, curr_stroke_size * STROKE_SIZE);
    store_write_direct(block_addr + curr_stroke_size * STROKE_SIZE, (const uint8_t *const) &attr, 1);
    store_write_direct(block_addr + curr_stroke_size * STROKE_SIZE + 1, entry_buf, entry_buf_len);
    const uint32_t bucket_addr = find_strokes((uint8_t *) curr_stroke, curr_stroke_size, 1);
    const uint32_t bucket = (uint32_t) entry_buf_len << 24 | ((block_addr - KVPAIR_BLOCK_START) & 0xFFFFF0) | (curr_stroke_size & 0x0F);
    store_write_direct(bucket_addr, (const uint8_t *const) &bucket, BUCKET_SIZE);
    store_flush();
#ifdef STENO_DEBUG_FLASH
    flash_debug_enable = 0;
#endif
    return false;
}

static void dicted_add_done(void) {
    entry_buf[entry_buf_len] = 0;
#ifdef STENO_DEBUG_DICTED
    steno_debug("Adding done with stroke: ");
    for (uint8_t i = 0; i < curr_stroke_size; i++) {
        const uint32_t stroke = STROKE_FROM_PTR(&curr_stroke[STROKE_SIZE * i]);
        char buf[24];
        stroke_to_string(stroke, buf, NULL);
        steno_debug("%s/", buf);
    }
    steno_debug_ln("\nAdding done with translation: %s", entry_buf);
#endif
    if (add_entry()) {
        return;
    }
    disp_add_done();
}

static void remove_entry(void) {
#ifdef STENO_DEBUG_FLASH
    flash_debug_enable = 1;
#endif
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("Started remove_entry()");
#endif

    const uint32_t last_entry_addr = BUCKET_GET_ADDR(remove_bucket);
    store_erase_partial(last_entry_addr, entry_length);
    // TODO Erase bucket and relocate all the later buckets
    // TODO Free block in allocator

#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("Finished remove_entry()");
#endif
#ifdef STENO_DEBUG_FLASH
    flash_debug_enable = 0;
#endif
}

void dicted_remove_prompt_strokes(void) {
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("dicted_remove_prompt_strokes executed");
#endif
    editing_state = ED_ACTIVE_REMOVE;
    disp_remove_prompt_strokes();
    curr_stroke_size = 0;
}

void dicted_edit_prompt_strokes(void) {
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("dicted_edit_prompt_strokes executed");
#endif
    editing_state = ED_ACTIVE_EDIT_CONF_STROKES;
    disp_edit_prompt_strokes();
    curr_stroke_size = 0;
}

static void dicted_edit_conf_strokes(void) {
    if (curr_stroke_size == 0) {
        editing_state = ED_ERROR;
        disp_show_abort();
        return;
    }

#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("display stroke edit executed");
    steno_debug("removing: ");
    print_strokes(curr_stroke, curr_stroke_size);
#endif

    const uint32_t bucket = find_strokes((uint8_t *) curr_stroke, curr_stroke_size, 0);
    if (bucket == 0 || bucket == 0xFFFFFFFF) {
        editing_state = ED_ERROR;
        disp_show_noentry();
        return;
    }
    read_entry(bucket, kvpair_buf);
    const uint8_t entry_len = BUCKET_GET_ENTRY_LEN(bucket);
    const uint8_t stroke_byte_len = STROKE_SIZE * curr_stroke_size;
    char entry_trans[entry_len + 1];

    entry_length = stroke_byte_len + 1 + entry_len;

    memcpy(entry_trans, kvpair_buf + stroke_byte_len + 1, entry_len);
    entry_trans[entry_len] = 0;
    remove_bucket = bucket;
    disp_edit_conf_entry(entry_trans);
}

static void dicted_edit_prompt_trans(void) {
    remove_entry();
    extern uint8_t hist_ind;
    hist_get(hist_ind)->state.space = 0;
    disp_edit_prompt_trans();
    entry_buf_len = 0;
}

static void dicted_edit_done(void) {
    if (entry_buf_len == 0) {
        editing_state = ED_ERROR;
        disp_show_abort();
        return;
    }

#ifdef STENO_DEBUG_DICTED
    steno_debug("Editing done with stroke: ");
    print_strokes(curr_stroke, curr_stroke_size);
#endif
    entry_buf[entry_buf_len] = 0;
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("\nEditing done with translation: %s", entry_buf);
#endif
    if (add_entry()) {
        return;
    }
    disp_edit_done();
}

static void dicted_remove_conf_strokes(void) {
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("display stroke remove executed");
    steno_debug("removing: ");
    print_strokes(curr_stroke, curr_stroke_size);
#endif
    const uint32_t bucket = find_strokes((uint8_t *) curr_stroke, curr_stroke_size, 0);
    if (bucket == 0 || bucket == 0xFFFFFFFF) {
        editing_state = ED_ERROR;
        disp_show_noentry();
        return;
    }
    read_entry(bucket, kvpair_buf);
    const uint8_t entry_len = BUCKET_GET_ENTRY_LEN(bucket);
    const uint8_t stroke_byte_len = STROKE_SIZE * curr_stroke_size;
    char entry_trans[entry_len + 1];

    entry_length = stroke_byte_len + 1 + entry_len;

    memcpy(entry_trans, kvpair_buf + stroke_byte_len + 1, entry_len);
    entry_trans[entry_len] = 0;
    remove_bucket = bucket;
    disp_remove_conf_entry(entry_trans);
}

bool _handle_dict_editing(const uint32_t stroke) {
    switch (editing_state) {
    case ED_ACTIVE_ADD:
        if (stroke == STENO_R_R) {
            editing_state = ED_ACTIVE_ADD_TRANS;
            dicted_add_prompt_trans();
        } else {
            dicted_add_stroke(stroke);
        }
        return true;
    case ED_ACTIVE_ADD_TRANS:
        if (stroke == STENO_R_R) {
            editing_state = ED_IDLE;
            dicted_add_done();
            return true;
        }
        // `process_output()` will handle the translation and write to buffer
        return false;

    case ED_ERROR:
        if (stroke == STENO_R_R) {
            editing_state = ED_IDLE;
            disp_unshow_error();
        }
        return true;

    case ED_ACTIVE_REMOVE:
        if (stroke == STENO_R_R) {
            editing_state = ED_ACTIVE_REMOVE_TRANS;
            dicted_remove_conf_strokes();
        } else {
            dicted_add_stroke(stroke);
        }
        return true;

    case ED_ACTIVE_REMOVE_TRANS:
        if (stroke == STENO_R_R) {
            editing_state = ED_IDLE;
            remove_entry();
            disp_remove_done();
        }
        return true;

    case ED_ACTIVE_EDIT_CONF_STROKES:
        if (stroke == STENO_R_R) {
            editing_state = ED_ACTIVE_EDIT_TRANS;
            dicted_edit_conf_strokes();
        } else {
            dicted_add_stroke(stroke);
        }
        return true;

    case ED_ACTIVE_EDIT_TRANS:
        if (stroke == STENO_R_R) {
            editing_state = ED_ACTIVE_EDIT_CONF_TRANS;
            dicted_edit_prompt_trans();
        }
        return true;

    case ED_ACTIVE_EDIT_CONF_TRANS:
        if (stroke == STENO_R_R) {
            editing_state = ED_IDLE;
            dicted_edit_done();
            return true;
        }
        // `process_output()` will handle the translation and write to buffer
        return false;
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
