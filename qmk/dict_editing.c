#include <string.h>
#include "steno.h"
#include "keymap_steno.h"
#include "spi.h"
#include "flash.h"
#include <stdio.h>
#include "hist.h"
#include "lcd.h"
#include "dict_editing.h"
#include "stroke.h"

editing_state_t editing_state = ED_IDLE;
// Buffer for a page; used for unparsed/formatted entry and copying between pages
uint8_t page_buffer[FLASH_PP_SIZE];
uint8_t entry_buf_len = 0;
static uint8_t curr_stroke[30];
static uint8_t curr_stroke_size = 0;
static uint8_t entry_length = 0;

void dicted_add_prompt_strokes(void) {
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("add_start executed");
#endif
    editing_state = ED_ACTIVE_ADD;
    select_lcd();
    lcd_clear();
    lcd_puts_at(0, 0, "Stroke to add:\n", 2);
    unselect_lcd();
    curr_stroke_size = 0;
}

void set_stroke(uint32_t stroke) {
    if (curr_stroke_size == 0) {
        lcd_pos(0, 16);
    }
    char temp[24];
    stroke_to_string(stroke, temp, NULL);
    if (stroke == 0x1000) {
        if (curr_stroke_size != 0) {
            curr_stroke_size -= 1;
            const uint32_t temp_last_stroke = STROKE_FROM_PTR(&curr_stroke[STROKE_SIZE * curr_stroke_size]);
            stroke_to_string(temp_last_stroke, temp, NULL);
            select_lcd();
            for (uint8_t i = 0; i < strlen(temp) + 1; i++) {
                lcd_back(2);
            }
            unselect_lcd();
        }
    } else if (curr_stroke_size < 10) {
        if (curr_stroke_size > 0) {
            lcd_puts("/", 2);
        }
        curr_stroke[STROKE_SIZE * curr_stroke_size + 2] = (stroke >> 16) & 0xFF;
        curr_stroke[STROKE_SIZE * curr_stroke_size + 1] = (stroke >> 8) & 0xFF;
        curr_stroke[STROKE_SIZE * curr_stroke_size] = stroke & 0xFF;
        curr_stroke_size++;
        // char last_stroke[24];
        // stroke_to_string(stroke, last_stroke, NULL);
        select_lcd();
#ifdef STENO_DEBUG_DICTED
        steno_debug_ln("set stroke passed");
#endif
        lcd_puts(temp, 2);
        unselect_lcd();
    }
}

void dicted_add_prompt_trans(void) {
    if (curr_stroke_size == 0) {
        editing_state = ED_ERROR;
        select_lcd();
        lcd_puts("\nAborted", 2);
        unselect_lcd();
        return;
    }

    find_strokes((uint8_t *) curr_stroke, curr_stroke_size, 0);
    uint8_t stroke_len = BUCKET_GET_STROKES_LEN(last_bucket);
    if (stroke_len != 0 && stroke_len != 0xFFFFFF) {
        editing_state = ED_ERROR;
        select_lcd();
        lcd_puts("\nEntry already exists", 2);
        unselect_lcd();
        return;
    }
    select_lcd();
    lcd_puts("\nEnter translation:\n", 2);
    extern uint8_t hist_ind;
    hist_get(hist_ind)->state.space = 0;
    unselect_lcd();
    entry_buf_len = 0;
}

void add_entry(void) {
#ifdef STENO_DEBUG_FLASH
    flash_debug_enable = 1;
#endif
    if (entry_buf_len == 0) {
        select_lcd();
        lcd_puts("\nAborted", 2);
        editing_state = ED_ERROR;
        unselect_lcd();
        return;
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
        select_lcd();
        lcd_puts("\nNo storage", 2);
        editing_state = ED_ERROR;
        unselect_lcd();
        return;
    }
    const uint32_t block_addr = block_ind * 16 + KVPAIR_BLOCK_START;
    attr_t attr = { .space_prev = 1, .space_after = 1, .glue = 0 };
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("blok addr %06lX", block_addr);
#endif
    flash_write(block_addr, (uint8_t *) curr_stroke, curr_stroke_size * STROKE_SIZE);
    flash_write(block_addr + curr_stroke_size * STROKE_SIZE, (uint8_t *) &attr, 1);
    flash_write(block_addr + curr_stroke_size * STROKE_SIZE + 1, page_buffer, entry_buf_len);
    // FIXME Special usage
    find_strokes((uint8_t *) curr_stroke, curr_stroke_size, 1);
    const uint32_t bucket_addr = last_bucket;
    last_bucket = 0;
    const uint32_t bucket = (uint32_t) entry_buf_len << 24 | ((block_addr - KVPAIR_BLOCK_START) & 0xFFFFF0) | (curr_stroke_size & 0x0F);
    flash_write(bucket_addr, (uint8_t *) &bucket, BUCKET_SIZE);
    flash_flush();
#ifdef STENO_DEBUG_FLASH
    flash_debug_enable = 0;
#endif
}

void dicted_add_done(void) {
    page_buffer[entry_buf_len] = 0;
#ifdef STENO_DEBUG_DICTED
    steno_debug("Adding done with stroke: ");
    for (uint8_t i = 0; i < curr_stroke_size; i++) {
        const uint32_t stroke = STROKE_FROM_PTR(&curr_stroke[STROKE_SIZE * i]);
        char buf[24];
        stroke_to_string(stroke, buf, NULL);
        steno_debug("%s/", buf);
    }
    steno_debug_ln("\nAdding done with translation: %s", page_buffer);
#endif
    add_entry();
    select_lcd();
    lcd_clear();
    unselect_lcd();
}

void dicted_remove_prompt_strokes(void) {
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("dicted_remove_prompt_strokes executed");
#endif
    editing_state = ED_ACTIVE_REMOVE;
    select_lcd();
    lcd_fill_rect(0, 0, LCD_WIDTH, 48, LCD_WHITE);
    lcd_puts_at(0, 0, "Remove stroke:\n", 2);
    unselect_lcd();
    curr_stroke_size = 0;
}

void dicted_edit_prompt_strokes(void) {
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("dicted_edit_prompt_strokes executed");
#endif
    editing_state = ED_ACTIVE_EDIT_CONF_STROKES;
    select_lcd();
    lcd_clear();
    lcd_puts_at(0, 0, "Edit stroke:\n", 2);
    unselect_lcd();
    curr_stroke_size = 0;
}

void dicted_edit_conf_strokes(void) {
    if (curr_stroke_size == 0) {
        editing_state = ED_ERROR;
        select_lcd();
        lcd_puts("\nAborted", 2);
        unselect_lcd();
        return;
    }

#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("display stroke edit executed");
    steno_debug("removing: ");
    print_strokes(curr_stroke, curr_stroke_size);
#endif

    find_strokes((uint8_t *) curr_stroke, curr_stroke_size, 0);
    // last_bucket is where the address is stored
    if (last_bucket == 0 || last_bucket == 0xFFFFFF) {
        editing_state = ED_ERROR;
        select_lcd();
        lcd_puts("\nNo Entry", 2);
        unselect_lcd();
        return;
    }
    read_entry(last_bucket, entry_buf);
    const uint8_t entry_len = BUCKET_GET_ENTRY_LEN(last_bucket);
    const uint8_t stroke_byte_len = STROKE_SIZE * curr_stroke_size;
    char entry_trans[entry_len + 1];

    entry_length = stroke_byte_len + 1 + entry_len;

    memcpy(entry_trans, entry_buf + stroke_byte_len + 1, entry_len);
    entry_trans[entry_len] = 0;

    select_lcd();
    lcd_puts("\nEntry to edit:\n", 2);
    lcd_puts(entry_trans, 2);
    lcd_puts("\nTo edit, press enter (R-R)", 2);
    unselect_lcd();
}

void dicted_edit_prompt_trans(void) {
    remove_entry();
    select_lcd();
    extern uint8_t hist_ind;
    hist_get(hist_ind)->state.space = 0;
    lcd_puts("\nEnter New translation:\n", 2);
    unselect_lcd();
    entry_buf_len = 0;
}

void dicted_edit_done(void) {
    if (entry_buf_len == 0) {
        editing_state = ED_ERROR;
        select_lcd();
        lcd_puts("\nAborted", 2);
        unselect_lcd();
        return;
    }

#ifdef STENO_DEBUG_DICTED
    steno_debug("Editing done with stroke: ");
    print_strokes(curr_stroke, curr_stroke_size);
#endif
    page_buffer[entry_buf_len] = 0;
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("\nEditing done with translation: %s", page_buffer);
#endif
    add_entry();
    select_lcd();
    lcd_clear();
    unselect_lcd();
}

void dicted_remove_conf_strokes(void) {
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("display stroke remove executed");
    steno_debug("removing: ");
    print_strokes(curr_stroke, curr_stroke_size);
#endif
    find_strokes((uint8_t *) curr_stroke, curr_stroke_size, 0);
    // last_bucket is where the address is stored
    if (last_bucket == 0 || last_bucket == 0xFFFFFF) {
        editing_state = ED_ERROR;
        select_lcd();
        lcd_puts("\nNo Entry", 2);
        unselect_lcd();
        return;
    }
    read_entry(last_bucket, entry_buf);
    const uint8_t entry_len = BUCKET_GET_ENTRY_LEN(last_bucket);
    const uint8_t stroke_byte_len = STROKE_SIZE * curr_stroke_size;
    char entry_trans[entry_len + 1];

    entry_length = stroke_byte_len + 1 + entry_len;

    memcpy(entry_trans, entry_buf + stroke_byte_len + 1, entry_len);
    entry_trans[entry_len] = 0;

    select_lcd();
    lcd_pos(0, 16);
    lcd_puts("Entry to remove:\n", 2);
    lcd_puts(entry_trans, 2);
    lcd_puts("\nTo remove, press enter (R-R)", 2);
    unselect_lcd();
}

void remove_entry(void) {
#ifdef STENO_DEBUG_FLASH
    flash_debug_enable = 1;
#endif
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("Started remove_entry()");
#endif

    const uint32_t last_entry_addr = BUCKET_GET_ADDR(last_bucket);
    const uint32_t block_addr_start = last_entry_addr & 0xFFF000; // Alighed to 4k
    flash_erase_4k(SCRATCH_START);

    const uint32_t last_entry_page_addr = last_entry_addr & 0xFFFF00; // Aligned to 256 (PP_SIZE)
    for (uint32_t address_ptr = block_addr_start, scratch_addr = SCRATCH_START; address_ptr < block_addr_start + 0x1000; address_ptr += FLASH_PP_SIZE) {
        flash_flush();
        flash_read_page(address_ptr, page_buffer);
        if (last_entry_page_addr == address_ptr) {
            const uint8_t offset = last_entry_addr & 0xFF;
            memset(page_buffer + offset, FLASH_ERASED_BYTE, entry_length);
#ifdef STENO_DEBUG_DICTED
            steno_debug_ln("cleared: offset: %02X, len: %02X", offset, entry_length);
#endif
        }
        flash_write_page(scratch_addr, page_buffer);
        scratch_addr += FLASH_PP_SIZE;
    }

    flash_erase_4k(block_addr_start);
    for (uint32_t address_ptr = block_addr_start, scratch_addr = SCRATCH_START; address_ptr < block_addr_start + 0x1000; address_ptr += FLASH_PP_SIZE) {
        flash_flush();
        flash_read_page(scratch_addr, page_buffer);
        flash_write_page(address_ptr, page_buffer);
        scratch_addr += FLASH_PP_SIZE;
    }
    // TODO Free block in allocator

#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("Finished remove_entry()");
#endif
    flash_flush();
#ifdef STENO_DEBUG_FLASH
    flash_debug_enable = 0;
#endif
}
