#include <string.h>
#include "steno.h"
#include "keymap_steno.h"
#include "raw_hid.h"
#include "spi.h"
#include "flash.h"
#include <stdio.h>
#include "hist.h"
#include "lcd.h"
#include "dict_editing.h"

editing_state_t editing_state = ED_IDLE;
char curr_stroke[30];
char curr_trans[64];
static uint8_t curr_stroke_size = 0;

void prompt_user(void) {
    steno_debug_ln("prompt_user executed");
    editing_state = ED_ACTIVE_ADD;
    select_lcd();
    lcd_fill_rect(0, 0, LCD_WIDTH, 48, LCD_WHITE);
    lcd_puts_at(0, 0, (uint8_t *)"Enter Stroke", 2);
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
            uint32_t temp_last_stroke = (uint32_t)curr_stroke[3 * curr_stroke_size] << 16 |
                                        (uint32_t)curr_stroke[3 * curr_stroke_size + 1] << 8 |
                                        (uint32_t)curr_stroke[3 * curr_stroke_size + 2];
            stroke_to_string(temp_last_stroke, temp, NULL);
            select_lcd();
            for (uint8_t i = 0; i < strlen(temp) + 1; i++) {
                lcd_back(2);
            }
            unselect_lcd();
        }
    } else if (curr_stroke_size < 9) {
        if (curr_stroke_size > 0) {
            lcd_puts((uint8_t *)"/", 2);
        }
        curr_stroke[3 * curr_stroke_size] = (stroke >> 16) & 0xFF;
        curr_stroke[3 * curr_stroke_size + 1] = (stroke >> 8) & 0xFF;
        curr_stroke[3 * curr_stroke_size + 2] = stroke & 0xFF;
        curr_stroke_size++;
        // char last_stroke[24];
        // stroke_to_string(stroke, last_stroke, NULL);
        select_lcd();
        steno_debug_ln("set stroke passed");
        lcd_puts((uint8_t *)temp, 2);
        unselect_lcd();
    }
}

void prompt_user_translation(void) {
    select_lcd();
    lcd_puts_at(0, 32, (uint8_t *)"Enter Translation", 2);
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
        lcd_puts((uint8_t *)trans, 2);
        unselect_lcd();
    }
}

void add_finished(void) {
    steno_debug_ln("Adding done with stroke: ");
    for (uint8_t i = 0; i < curr_stroke_size; i++) {
        steno_debug("%X%X%X /a", curr_stroke[i * 3], curr_stroke[i * 3 + 1], curr_stroke[i * 3 + 2]);
    }
    steno_debug_ln("Adding done with translation: %s", curr_trans);
    select_lcd();
    lcd_fill_rect(0, 0, LCD_WIDTH, LCD_HEIGHT, LCD_WHITE);
    unselect_lcd();
}