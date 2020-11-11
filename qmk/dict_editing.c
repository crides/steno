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
static char curr_stroke[30];
static uint8_t curr_stroke_size = 0;
static uint8_t entry_length = 0;

void prompt_user(void) {
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("prompt_user executed");
#endif
    editing_state = ED_ACTIVE_ADD;
    select_lcd();
    lcd_clear();
    lcd_puts_at(0, 0, "Stroke to add:\n", 2);
    unselect_lcd();
    curr_stroke_size = 0;
}

void set_Stroke(uint32_t stroke) {
    if (curr_stroke_size == 0) {
        lcd_pos(0, 16);
    }
    char temp[24];
    stroke_to_string(stroke, temp, NULL);
    if (stroke == 0x1000) {
        if (curr_stroke_size != 0) {
            curr_stroke_size -= 1;
            uint32_t temp_last_stroke = (uint32_t) curr_stroke[3 * curr_stroke_size + 2] << 16 |
                                        (uint32_t) curr_stroke[3 * curr_stroke_size + 1] << 8 |
                                        (uint32_t) curr_stroke[3 * curr_stroke_size];
            stroke_to_string(temp_last_stroke, temp, NULL);
            select_lcd();
            for (uint8_t i = 0; i < strlen(temp) + 1; i++) {
                lcd_back(2);
            }
            unselect_lcd();
        }
    } else if (curr_stroke_size < 9) {
        if (curr_stroke_size > 0) {
            lcd_puts("/", 2);
        }
        curr_stroke[3 * curr_stroke_size + 2] = (stroke >> 16) & 0xFF;
        curr_stroke[3 * curr_stroke_size + 1] = (stroke >> 8) & 0xFF;
        curr_stroke[3 * curr_stroke_size] = stroke & 0xFF;
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

void prompt_user_translation(void) {
    select_lcd();
    lcd_puts("\nEnter Translation:\n", 2);
    unselect_lcd();
    entry_buf_len = 0;
}

void add_finished(void) {
#ifdef STENO_DEBUG_DICTED
    steno_debug("Adding done with stroke: ");
    for (uint8_t i = 0; i < curr_stroke_size; i++) {
        uint32_t stroke = (uint32_t) curr_stroke[3 * i + 2] << 16 | (uint32_t) curr_stroke[3 * i + 1] << 8 |
                          (uint32_t) curr_stroke[3 * i];
        char buf[24];
        stroke_to_string(stroke, buf, NULL);
        steno_debug("%s/", buf);
    }
#endif
    page_buffer[entry_buf_len] = 0;
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("\nAdding done with translation: %s", page_buffer);
#endif
    select_lcd();
    lcd_clear();
    unselect_lcd();
}

void prompt_user_remove(void) {
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("prompt_user_remove executed");
#endif
    editing_state = ED_ACTIVE_REMOVE;
    select_lcd();
    lcd_fill_rect(0, 0, LCD_WIDTH, 48, LCD_WHITE);
    lcd_puts_at(0, 0, "Enter Stroke:\n", 2);
    unselect_lcd();
    curr_stroke_size = 0;
}

void prompt_user_edit(void) {
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("prompt_user_edit executed");
#endif
    editing_state = ED_ACTIVE_EDIT;
    select_lcd();
    lcd_fill_rect(0, 0, LCD_WIDTH, 48, LCD_WHITE);
    lcd_puts_at(0, 0, "Enter Stroke:\n", 2);
    unselect_lcd();
    memset(curr_stroke, 0, 30);
}

void display_stroke_to_edit(void) {
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("display stroke edit executed");
#endif

    find_strokes((uint8_t *) curr_stroke, curr_stroke_size);
    // last_entry_ptr is where the address is stored
    address_ptr = ENTRY_GET_ADDR(last_entry_ptr);
    if (last_entry_ptr == 0 || last_entry_ptr == 0xFFFFFF) {
        editing_state = ED_ERROR;
        select_lcd();
        lcd_pos(0, 16);
        lcd_puts("No Entry", 2);
        unselect_lcd();
        return;
    }
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("address_ptr = %lX", address_ptr);
#endif
    uint8_t stroke_byte_len = 3 * curr_stroke_size;
    flash_read(address_ptr, entry_buf, stroke_byte_len + 1);
    uint8_t entry_len = entry_buf[stroke_byte_len];
    flash_read(address_ptr + stroke_byte_len + 1, entry_buf + stroke_byte_len + 1, entry_len + 1);
    char entry_trans[entry_len];

    entry_length = stroke_byte_len + 1 + entry_len + 1;

    memcpy(entry_trans, entry_buf + stroke_byte_len + 2, entry_len);

    select_lcd();
    lcd_pos(0, 16);
    lcd_puts("Entry to edit:\n", 2);
    lcd_puts(entry_trans, 2);
    lcd_puts("\nTo edit, press enter (R-R)", 2);
    unselect_lcd();
}

void prompt_user_edit_translation(void) {
    remove_stroke();
    select_lcd();
    lcd_pos(0, 64);
    lcd_puts("\nEnter New Translation:\n", 2);
    unselect_lcd();
    /* memset(page_buffer, 0, FLASH_PP_SIZE); */
    entry_buf_len = 0;
}

void edit_finished(void) {
#ifdef STENO_DEBUG_DICTED
    steno_debug("Editing done with stroke: ");
#endif
    for (uint8_t i = 0; i < curr_stroke_size; i++) {
        uint32_t stroke = (uint32_t) curr_stroke[3 * i + 2] << 16 | (uint32_t) curr_stroke[3 * i + 1] << 8 |
                          (uint32_t) curr_stroke[3 * i];
#ifdef STENO_DEBUG_DICTED
        char buf[24];
        stroke_to_string(stroke, buf, NULL);
        steno_debug("%s/", buf);
#endif
    }
    page_buffer[entry_buf_len] = 0;
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("\nEditing done with translation: %s", page_buffer);
#endif
    select_lcd();
    lcd_clear();
    unselect_lcd();
}

void display_stroke_to_remove(void) {
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("display stroke remove executed");
#endif

#ifdef STENO_DEBUG_DICTED
    steno_debug("removing: ");
    for (uint8_t i = 0; i < curr_stroke_size; i++) {
        uint32_t stroke = (uint32_t) curr_stroke[3 * i + 2] << 16 | (uint32_t) curr_stroke[3 * i + 1] << 8 |
                          (uint32_t) curr_stroke[3 * i];
        char buf[24];
        stroke_to_string(stroke, buf, NULL);
        steno_debug("%s/", buf);
    }
#endif
    find_strokes((uint8_t *) curr_stroke, curr_stroke_size);
    // last_entry_ptr is where the address is stored
    if (last_entry_ptr == 0 || last_entry_ptr == 0xFFFFFF) {
        editing_state = ED_ERROR;
        select_lcd();
        lcd_pos(0, 16);
        lcd_puts("No Entry", 2);
        unselect_lcd();
        return;
    }
    const uint32_t last_entry_addr = ENTRY_GET_ADDR(last_entry_ptr);
#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("last_entry_addr = %lX", last_entry_addr);
#endif
    uint8_t stroke_byte_len = 3 * curr_stroke_size;
    flash_read(last_entry_addr, entry_buf, stroke_byte_len + 1);
    uint8_t entry_len = entry_buf[stroke_byte_len];
    flash_read(last_entry_addr + stroke_byte_len + 1, entry_buf + stroke_byte_len + 1, entry_len + 1);
    char entry_trans[entry_len + 1];

    entry_length = stroke_byte_len + 1 + entry_len + 1;
    steno_debug_ln("entry_length: %02X", entry_length);

    memcpy(entry_trans, entry_buf + stroke_byte_len + 2, entry_len);
    entry_trans[entry_len] = 0;

    select_lcd();
    lcd_pos(0, 16);
    lcd_puts("Entry to remove:\n", 2);
    lcd_puts(entry_trans, 2);
    lcd_puts("\nTo remove, press enter (R-R)", 2);
    unselect_lcd();
}

void remove_stroke(void) {

#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("Started remove_stroke()");
#endif

    const uint32_t last_entry_addr = ENTRY_GET_ADDR(last_entry_ptr);
    const uint32_t block_addr_start = last_entry_addr & 0xFFF000; // Alighed to 4k
    flash_erase_4k(SCRATCH_START);

    const uint32_t last_entry_page_addr = last_entry_addr & 0xFFFF00; // Aligned to 256 (PP_SIZE)
    for (uint32_t address_ptr = block_addr_start, scratch_addr = SCRATCH_START; address_ptr < block_addr_start + 0x1000; address_ptr += FLASH_PP_SIZE) {
        flash_read_page(address_ptr, page_buffer);
        if (last_entry_page_addr == address_ptr) {
            uint8_t offset = last_entry_addr & 0xFF;
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
        flash_read_page(scratch_addr, page_buffer);
        flash_write_page(address_ptr, page_buffer);
        scratch_addr += FLASH_PP_SIZE;
    }

    select_lcd();
    lcd_clear();
    unselect_lcd();

#ifdef STENO_DEBUG_DICTED
    steno_debug_ln("Finished remove_stroke()");
#endif
}
