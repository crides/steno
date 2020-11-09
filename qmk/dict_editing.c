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
char curr_stroke[30];
char curr_trans[64];
static uint8_t curr_stroke_size = 0;
uint32_t address_ptr = 0;
uint8_t entry_length = 0;

void prompt_user(void) {
    steno_debug_ln("prompt_user executed");
    editing_state = ED_ACTIVE_ADD;
    select_lcd();
    lcd_fill_rect(0, 0, LCD_WIDTH, 48, LCD_WHITE);
    lcd_puts_at(0, 0, (uint8_t *) "Enter Stroke to add: ", 2);
    unselect_lcd();
    memset(curr_stroke, 0, 30);
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
            lcd_puts((uint8_t *) "/", 2);
        }
        curr_stroke[3 * curr_stroke_size + 2] = (stroke >> 16) & 0xFF;
        curr_stroke[3 * curr_stroke_size + 1] = (stroke >> 8) & 0xFF;
        curr_stroke[3 * curr_stroke_size] = stroke & 0xFF;
        curr_stroke_size++;
        // char last_stroke[24];
        // stroke_to_string(stroke, last_stroke, NULL);
        select_lcd();
        steno_debug_ln("set stroke passed");
        lcd_puts((uint8_t *) temp, 2);
        unselect_lcd();
    }
}

void prompt_user_translation(void) {
    select_lcd();
    lcd_puts_at(0, 32, (uint8_t *) "Enter Translation", 2);
    unselect_lcd();
    memset(curr_trans, 0, 64);
}

void set_Trans(char trans[]) {
    // char * temp = (char *) malloc(1 + strlen(curr_trans)+ strlen(trans) );
    // strcpy(temp, curr_trans);
    // strcat(temp, trans);
    // steno_debug_ln("curr_trans = " + *temp);
    static uint8_t curr_trans_index = 0;
    if (curr_trans_index == 0) {
        lcd_pos(0, 48);
    }
    if (strcmp(trans, "*") == 0) {
        if (curr_trans_index != 0) {
            curr_trans_index--;
            select_lcd();
            lcd_back(2);
            unselect_lcd();
        }
    } else {
        strcat(curr_trans, trans);
        curr_trans_index += strlen(trans);
        select_lcd();
        lcd_puts((uint8_t *) trans, 2);
        unselect_lcd();
    }
}

void add_finished(void) {
    char buf[24];
    steno_debug("Adding done with stroke: ");
    for (uint8_t i = 0; i < curr_stroke_size; i++) {
        uint32_t stroke = (uint32_t) curr_stroke[3 * i + 2] << 16 | (uint32_t) curr_stroke[3 * i + 1] << 8 |
                          (uint32_t) curr_stroke[3 * i];
        stroke_to_string(stroke, buf, NULL);
        steno_debug("%s/", buf);
    }
    steno_debug_ln("\nAdding done with translation: %s", curr_trans);
    select_lcd();
    lcd_fill_rect(0, 0, LCD_WIDTH, LCD_HEIGHT, LCD_WHITE);
    unselect_lcd();

    curr_stroke_size = 0;
}

void prompt_user_remove(void) {
    steno_debug_ln("prompt_user_remove executed");
    editing_state = ED_ACTIVE_REMOVE;
    select_lcd();
    lcd_fill_rect(0, 0, LCD_WIDTH, 48, LCD_WHITE);
    lcd_puts_at(0, 0, (uint8_t *) "Enter Stroke: ", 2);
    unselect_lcd();
    memset(curr_stroke, 0, 30);
}

void display_stroke_to_remove(void) {
    steno_debug_ln("display stroke remove executed");

    find_strokes((uint8_t *) curr_stroke, curr_stroke_size);
    // last_entry_ptr is where the address is stored
    address_ptr = ENTRY_GET_ADDR(last_entry_ptr);
    if (last_entry_ptr == 0 || last_entry_ptr == 0xFFFFFF) {
        editing_state = ED_ERROR;
        select_lcd();
        lcd_pos(0, 16);
        lcd_puts((uint8_t *) "No Entry", 2);
        unselect_lcd();
        return;
    }
    steno_debug_ln("address_ptr = %lX", address_ptr);
    uint8_t stroke_byte_len = 3 * curr_stroke_size;
    flash_read(address_ptr, entry_buf, stroke_byte_len + 1);
    uint8_t entry_len = entry_buf[stroke_byte_len];
    flash_read(address_ptr + stroke_byte_len + 1, entry_buf + stroke_byte_len + 1, entry_len + 1);
    char entry_trans[entry_len];

    entry_length = stroke_byte_len + 1 + entry_len + 1;

    memcpy(entry_trans, entry_buf + stroke_byte_len + 2, entry_len);

    select_lcd();
    lcd_pos(0, 16);
    lcd_puts((uint8_t *) "Entry to remove: ", 2);
    lcd_puts((uint8_t *) entry_trans, 2);
    lcd_puts_at(0, 48, (uint8_t *) "To remove, press enter (R/R)", 2);
    unselect_lcd();
}

void remove_stroke(void) {

    steno_debug_ln("Started remove_stroke()");

    address_ptr = address_ptr & 0xFFF000;

    // scratchpage is at 0xF22000 to 0xF23000
    uint32_t scratch_addr = SCRATCH_START;
    uint8_t temp_scratch[256];
    flash_erase_4k(scratch_addr);

    uint32_t apt = address_ptr;

    for (; address_ptr < apt + 0x1000; address_ptr += 256) {

        flash_read_page(address_ptr, temp_scratch);
        if (address_ptr >= ENTRY_GET_ADDR(last_entry_ptr) ||
            address_ptr < ENTRY_GET_ADDR(last_entry_ptr) + entry_length) {
            uint32_t offset = address_ptr - ENTRY_GET_ADDR(last_entry_ptr);
            memset(temp_scratch + offset, 0xFF, entry_length);
            flash_write_page(scratch_addr, temp_scratch);

        } else {
            flash_write_page(scratch_addr, temp_scratch);
            scratch_addr += 256;
        }
    }
    address_ptr = apt;
    flash_erase_4k(address_ptr);
    for (; address_ptr < apt + 0x1000; address_ptr += 256) {
        flash_read_page(scratch_addr, temp_scratch);
        scratch_addr += 256;
        flash_write_page(address_ptr, temp_scratch);
    }

    select_lcd();
    lcd_fill_rect(0, 0, LCD_WIDTH, LCD_HEIGHT, LCD_WHITE);
    unselect_lcd();

    steno_debug_ln("Finished remove_stroke()");
}
