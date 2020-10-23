#include <string.h>
#include "steno.h"
#include "keymap_steno.h"
#include "spi.h"
#include "flash.h"
#include <stdio.h>
#include "hist.h"
#include "lcd.h"
#include "dict_editing.h"

editing_state_t editing_state = ED_IDLE;
char * curr_stroke;

void prompt_user(void){
	steno_debug_ln("prompt_user executed");
	editing_state = ED_ACTIVE_ADD;
	select_lcd();
	lcd_fill_rect(0, 0, LCD_WIDTH, 48, LCD_WHITE);
	lcd_puts(0, 0, (uint8_t*)"Enter Stroke", 2);
	unselect_lcd();
}

void set_Stroke(char stroke[]){
	curr_stroke = stroke;
	select_lcd();
	lcd_puts(0, 16, (uint8_t *)curr_stroke, 2);
	if(editing_state == ED_ACTIVE_ADD){
		lcd_puts(0, 32, (uint8_t*)"Enter Translation", 2);
	}
	unselect_lcd();
}
