#include "lcd.h"
#include "spi.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdbool.h>

extern const unsigned char font[] PROGMEM;

static const uint8_t PROGMEM initcmd[] = {
  0xEF, 3, 0x03, 0x80, 0x02,
  0xCF, 3, 0x00, 0xC1, 0x30,
  0xED, 4, 0x64, 0x03, 0x12, 0x81,
  0xE8, 3, 0x85, 0x00, 0x78,
  0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
  0xF7, 1, 0x20,
  0xEA, 2, 0x00, 0x00,
  ILI9341_PWCTR1  , 1, 0x23,             // Power control VRH[5:0]
  ILI9341_PWCTR2  , 1, 0x10,             // Power control SAP[2:0];BT[3:0]
  ILI9341_VMCTR1  , 2, 0x3e, 0x28,       // VCM control
  ILI9341_VMCTR2  , 1, 0x86,             // VCM control2
  ILI9341_MADCTL  , 1, 0x48,             // Memory Access Control
  ILI9341_VSCRSADD, 1, 0x00,             // Vertical scroll zero
  ILI9341_PIXFMT  , 1, 0x55,
  ILI9341_FRMCTR1 , 2, 0x00, 0x18,
  ILI9341_DFUNCTR , 3, 0x08, 0x82, 0x27, // Display Function Control
  0xF2, 1, 0x00,                         // 3Gamma Function Disable
  ILI9341_GAMMASET , 1, 0x01,             // Gamma curve selected
  ILI9341_GMCTRP1 , 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
    0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
  ILI9341_GMCTRN1 , 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
    0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
  ILI9341_SLPOUT  , 0x80,                // Exit Sleep
  ILI9341_DISPON  , 0x80,                // Display on
  0x00                                   // End of list
};

static int16_t cursor_x = 0, cursor_y = 0;
// NOTE assuming size == 2
static uint16_t dirty_lines = 0;

void set_addr_window(const uint16_t x1, const uint16_t y1, const uint16_t w, const uint16_t h) {
    const uint16_t x2 = (x1 + w - 1), y2 = (y1 + h - 1);
    lcd_command();
    spi_send_byte(ILI9341_CASET); // Column address set
    lcd_data();
    spi_send_word(x1);
    spi_send_word(x2);
    lcd_command();
    spi_send_byte(ILI9341_PASET); // Row address set
    lcd_data();
    spi_send_word(y1);
    spi_send_word(y2);
    lcd_command();
    spi_send_byte(ILI9341_RAMWR); // Write to RAM
    lcd_data();
}

void lcd_pos(const uint16_t x, const uint16_t y) {
    cursor_x = x;
    cursor_y = y;
}

static void lcd_draw_pixel(const uint16_t x, const uint16_t y, const uint16_t color) {
    if (x < LCD_WIDTH && y < LCD_HEIGHT) {
        set_addr_window(x, y, 1, 1);
        spi_send_word(color);
    }
}

#ifdef STENO_STROKE_DISPLAY
static void lcd_draw_hline(const int16_t x, const int16_t y, const int16_t w, const int16_t color) {
    set_addr_window(x, y, w, 1);
    for (int16_t i = 0; i < w; i++) {
        spi_send_word(color);
    }
}
#endif

static void lcd_draw_vline(const int16_t x, const int16_t y, const int16_t h, const int16_t color) {
    set_addr_window(x, y, 1, h);
    for (int16_t i = 0; i < h; i++) {
        spi_send_word(color);
    }
}

void lcd_fill_rect(const int16_t x, const int16_t y, const int16_t w, const int16_t h, const uint16_t color) {
    set_addr_window(x, y, w, h);
    for (int16_t i = 0; i < h; i++) {
        for (int16_t j = 0; j < w; j++) {
            spi_send_word(color);
        }
    }
}

static void lcd_draw_char(const int16_t x, const int16_t y, char c, const uint8_t size, const uint16_t fg, const uint16_t bg) {
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT || (x + LCD_FONT_WIDTH - 1) < 0 || (y + LCD_FONT_HEIGHT - 1) < 0 || c < ' ') {
        return;
    }

    c -= ' ';
#if 0
    if (c < 0 || c > ('~' - ' ')) {
        steno_error_ln("Attempting to draw out of range character: %02X", c + ' ');
        return;
    }
#endif

    select_lcd();
    for (int8_t i = 0; i < 5; i++) { // Char bitmap = 5 columns
        uint8_t line = pgm_read_byte(&font[c * 5 + i]);
        for (int8_t j = 0; j < LCD_FONT_HEIGHT; j++, line >>= 1) {
            if (line & 1) {
                if (size == 1) {
                    lcd_draw_pixel(x + i, y + j, fg);
                } else {
                    lcd_fill_rect(x + i * size, y + j * size, size, size, fg);
                }
            } else if (bg != fg) {
                if (size == 1) {
                    lcd_draw_pixel(x + i, y + j, bg);
                } else {
                    lcd_fill_rect(x + i * size, y + j * size, size, size, bg);
                }
            }
        }
    }
    if (bg != fg) { // If opaque, draw vertical line for last column
        if (size == 1) {
            lcd_draw_vline(x + 5, y, LCD_FONT_HEIGHT, bg);
        } else {
            lcd_fill_rect(x + 5 * size, y, size, LCD_FONT_HEIGHT * size, bg);
        }
    }
    unselect_lcd();
}

void lcd_putc(char c, uint8_t size) {
    dirty_lines |= (uint16_t) 1 << (cursor_y / 16);
    if (c == '\n') {                             // Newline?
        cursor_x = 0;                            // Reset x to zero,
        cursor_y += size * LCD_FONT_HEIGHT;      // advance y one line
    } else if (c != '\r') {                      // Ignore carriage returns
        if ((cursor_x + size * LCD_FONT_WIDTH) > LCD_WIDTH) { // Off right?
            cursor_x = 0;                        // Reset x to zero,
            cursor_y += size * LCD_FONT_HEIGHT;  // advance y one line
        }
        lcd_draw_char(cursor_x, cursor_y, c, size, LCD_BLACK, LCD_WHITE);
        cursor_x += size * LCD_FONT_WIDTH; // Advance x one char
    }
}

void lcd_puts_at(const int16_t x, const int16_t y, const char *str, uint8_t size) {
    cursor_x = x;
    cursor_y = y;
    while (*str) {
        lcd_putc(*str, size);
        str++;
    }
}

void lcd_puts(const char *str, const uint8_t size) {
    while (*str) {
        lcd_putc(*str, size);
        str++;
    }
}

void lcd_back(const uint8_t size) {
    if (cursor_x == 0) {
        if (cursor_y != 0) {
            cursor_x = LCD_WIDTH - LCD_FONT_WIDTH * size;
            cursor_y -= LCD_FONT_HEIGHT * size;
        } else {
            return;
        }
    } else {
        cursor_x = cursor_x - LCD_FONT_WIDTH * size;
    }
    lcd_fill_rect(cursor_x, cursor_y, size * LCD_FONT_WIDTH, size * LCD_FONT_HEIGHT, LCD_WHITE);
}

void lcd_clear_dirty(void) {
    for (uint8_t i = 0; i < 20; i ++) {
        if (dirty_lines & ((uint16_t) 1 << i)) {
            lcd_fill_rect(0, i * 16, LCD_WIDTH, i * 16 + 16, LCD_WHITE);
        }
    }
    dirty_lines = 0;
}

void lcd_clear(void) {
    lcd_fill_rect(0, 0, LCD_WIDTH, LCD_HEIGHT, LCD_WHITE);
}

#ifdef STENO_STROKE_DISPLAY
#define FRAME_WIDTH 20
#define FRAME_HEIGHT 24
#define FRAME_OFFSET 3

// FIXME Fix the math
static void lcd_stenotype_frame_single(const int16_t x, const int16_t y) {
    lcd_draw_hline(x + 1, y, FRAME_WIDTH - 2, LCD_BLACK);
    lcd_draw_vline(x, y, FRAME_HEIGHT, LCD_BLACK);
    lcd_draw_vline(x + FRAME_WIDTH - 1, y, FRAME_HEIGHT, LCD_BLACK);
    lcd_draw_hline(x + 1, y + FRAME_HEIGHT - 1, FRAME_WIDTH - 2, LCD_BLACK);
}

static void lcd_stenotype_frame_left_s(const int16_t x, const int16_t y) {
    lcd_draw_hline(x + 1, y, FRAME_WIDTH - 2, LCD_BLACK);
    lcd_draw_vline(x, y, FRAME_HEIGHT * 2, LCD_BLACK);
    lcd_draw_vline(x + FRAME_WIDTH - 1, y, FRAME_HEIGHT * 2, LCD_BLACK);
    lcd_draw_hline(x + 1, y + FRAME_HEIGHT * 2 - 1, FRAME_WIDTH - 2, LCD_BLACK);
}

static void lcd_stenotype_frame_star(const int16_t x, const int16_t y) {
    lcd_draw_hline(x + 1, y, 2 * FRAME_WIDTH - 2, LCD_BLACK);
    lcd_draw_vline(x, y, FRAME_HEIGHT * 2, LCD_BLACK);
    lcd_draw_vline(x + 2 * FRAME_WIDTH - 1, y, FRAME_HEIGHT * 2, LCD_BLACK);
    lcd_draw_hline(x + 1, y + FRAME_HEIGHT * 2 - 1, 2 * FRAME_WIDTH - 2, LCD_BLACK);
}

static void lcd_stenotype_frame(void) {
    const int16_t first_y = LCD_HEIGHT - 3 * FRAME_HEIGHT;
    const int16_t second_y = LCD_HEIGHT - 2 * FRAME_HEIGHT;
    const int16_t thumb_y = LCD_HEIGHT - FRAME_HEIGHT;
    lcd_stenotype_frame_left_s(FRAME_WIDTH, first_y);
    for (uint8_t i = 2; i < 5; i ++) {
        lcd_stenotype_frame_single(i * FRAME_WIDTH, first_y);
        lcd_stenotype_frame_single(i * FRAME_WIDTH, second_y);
    }
    lcd_stenotype_frame_star(5 * FRAME_WIDTH, first_y);
    for (uint8_t i = 7; i < 12; i ++) {
        lcd_stenotype_frame_single(i * FRAME_WIDTH, first_y);
        lcd_stenotype_frame_single(i * FRAME_WIDTH, second_y);
    }
    for (uint8_t i = 0; i < 6; i ++) {
        lcd_stenotype_frame_single((i + 3) * FRAME_WIDTH, thumb_y);
    }
}

static void lcd_stenotype_update_draw_key(const int16_t x, const int16_t y, const char c, const bool on) {
    const uint16_t fg = on ? LCD_WHITE : LCD_BLACK;
    const uint16_t bg = on ? LCD_BLACK : LCD_WHITE;
    lcd_draw_char(x + 1 + FRAME_OFFSET, y + 1 + FRAME_OFFSET, c, 2, fg, bg);
    select_lcd();
    lcd_fill_rect(x + 1, y + 1, FRAME_WIDTH - 2, FRAME_OFFSET, bg);
    lcd_fill_rect(x + 1, y + FRAME_HEIGHT - FRAME_OFFSET - 1, FRAME_WIDTH - 2, FRAME_OFFSET, bg);
    lcd_fill_rect(x + 1, y + FRAME_OFFSET + 1, FRAME_OFFSET, FRAME_HEIGHT - 2 - 2 * FRAME_OFFSET, bg);
    lcd_fill_rect(x + FRAME_WIDTH - FRAME_OFFSET - 1, y + FRAME_OFFSET + 1, FRAME_OFFSET, FRAME_HEIGHT - 2 - 2 * FRAME_OFFSET, bg);
    unselect_lcd();
}

static void lcd_stenotype_update_draw_left_s(const int16_t x, const int16_t y, const bool on) {
    const uint16_t fg = on ? LCD_WHITE : LCD_BLACK;
    const uint16_t bg = on ? LCD_BLACK : LCD_WHITE;
    lcd_draw_char(x + 1 + FRAME_OFFSET, y + 1 + FRAME_OFFSET + FRAME_HEIGHT / 2, 'S', 2, fg, bg);
    select_lcd();
    lcd_fill_rect(x + 1, y + 1, FRAME_WIDTH - 2, FRAME_OFFSET + FRAME_HEIGHT / 2, bg);
    lcd_fill_rect(x + 1, y + 2 * FRAME_HEIGHT - FRAME_OFFSET - 1 - FRAME_HEIGHT / 2, FRAME_WIDTH - 2, FRAME_OFFSET + FRAME_HEIGHT / 2, bg);
    lcd_fill_rect(x + 1, y + FRAME_OFFSET + 1, FRAME_OFFSET, 2 * FRAME_HEIGHT - 2 - 2 * FRAME_OFFSET, bg);
    lcd_fill_rect(x + FRAME_WIDTH - FRAME_OFFSET - 1, y + FRAME_OFFSET + 1, FRAME_OFFSET, 2 * FRAME_HEIGHT - 2 - 2 * FRAME_OFFSET, bg);
    unselect_lcd();
}

static void lcd_stenotype_update_draw_star(const int16_t x, const int16_t y, const bool on) {
    const uint16_t fg = on ? LCD_WHITE : LCD_BLACK;
    const uint16_t bg = on ? LCD_BLACK : LCD_WHITE;
    lcd_draw_char(x + 1 + FRAME_OFFSET + FRAME_WIDTH / 2, y + 1 + FRAME_OFFSET + FRAME_HEIGHT / 2, '*', 2, fg, bg);
    select_lcd();
    lcd_fill_rect(x + 1, y + 1, 2 * FRAME_WIDTH - 2, FRAME_OFFSET + FRAME_HEIGHT / 2, bg);
    lcd_fill_rect(x + 1, y + 2 * FRAME_HEIGHT - FRAME_OFFSET - 1 - FRAME_HEIGHT / 2, 2 * FRAME_WIDTH - 2, FRAME_OFFSET + FRAME_HEIGHT / 2, bg);
    lcd_fill_rect(x + 1, y + FRAME_OFFSET, FRAME_OFFSET + FRAME_WIDTH / 2, 2 * FRAME_HEIGHT - 2 - 2 * FRAME_OFFSET, bg);
    lcd_fill_rect(x + 2 * FRAME_WIDTH - FRAME_OFFSET - FRAME_WIDTH / 2 - 1, y + FRAME_OFFSET, FRAME_OFFSET + FRAME_WIDTH / 2, 2 * FRAME_HEIGHT - 2 - 2 * FRAME_OFFSET, bg);
    unselect_lcd();
}

static void lcd_stenotype_update_handle_key(const uint8_t num, const bool on) {
    switch (num) {
    case 22:    // #
        lcd_stenotype_update_draw_key(3 * FRAME_WIDTH, LCD_HEIGHT - FRAME_HEIGHT, '#', on);
        lcd_stenotype_update_draw_key(8 * FRAME_WIDTH, LCD_HEIGHT - FRAME_HEIGHT, '#', on);
        return;
    case 21:    // S
        lcd_stenotype_update_draw_left_s(FRAME_WIDTH, LCD_HEIGHT - 3 * FRAME_HEIGHT, on);
        return;
    case 12:    // *
        lcd_stenotype_update_draw_star(5 * FRAME_WIDTH, LCD_HEIGHT - 3 * FRAME_HEIGHT, on);
        return;
    }
    uint8_t x, y;
    // ST|KPWH|RAO*|EUFR|PBLG|TSDZ
    switch (num) {
    case 20: x = 2; y = 0; break;
    case 19: x = 2; y = 1; break;
    case 18: x = 3; y = 0; break;
    case 17: x = 3; y = 1; break;
    case 16: x = 4; y = 0; break;
    case 15: x = 4; y = 1; break;
    case 14: x = 4; y = 2; break;
    case 13: x = 5; y = 2; break;
    case 11: x = 6; y = 2; break;
    case 10: x = 7; y = 2; break;
    case 9: x = 7; y = 0; break;
    case 8: x = 7; y = 1; break;
    case 7: x = 8; y = 0; break;
    case 6: x = 8; y = 1; break;
    case 5: x = 9; y = 0; break;
    case 4: x = 9; y = 1; break;
    case 3: x = 10; y = 0; break;
    case 2: x = 10; y = 1; break;
    case 1: x = 11; y = 0; break;
    case 0: x = 11; y = 1; break;
    }
    const char *KEYS = "#STKPWHRAO*EUFRPBLGTSDZ";
    lcd_stenotype_update_draw_key(x * FRAME_WIDTH, y * FRAME_HEIGHT + LCD_HEIGHT - 3 * FRAME_HEIGHT, KEYS[23 - num - 1], on);
}

void lcd_stenotype_update(const uint32_t stroke) {
    static uint32_t last_stroke = 0xFFFFFF;
    const uint32_t diff = last_stroke ^ stroke;
    for (uint8_t i = 0; i < 23; i ++) {
        const uint32_t mask = (uint32_t) 1 << i;
        if (diff & mask) {
            lcd_stenotype_update_handle_key(i, (stroke & mask) > 0);
        }
    }
    last_stroke = stroke;
}
#endif

// SPI assume inited
void lcd_init(void) {
    configure_lcd_cs();
    unselect_lcd();
    uint8_t cmd, x, numArgs;
    const uint8_t *addr = initcmd;
    configure_lcd_cd();
    select_lcd();
    lcd_command();
    spi_send_byte(ILI9341_SWRESET);
    lcd_data();
    unselect_lcd();
    _delay_ms(150);

    while ((cmd = pgm_read_byte(addr++)) > 0) {
        x = pgm_read_byte(addr++);
        numArgs = x & 0x7F;
        select_lcd();
        lcd_command();
        spi_send_byte(cmd);
        lcd_data();
        for (uint8_t i = 0; i < numArgs; i++) {
            spi_send_byte(pgm_read_byte(addr + i));
        }
        unselect_lcd();
        addr += numArgs;
        if (x & 0x80) {
            _delay_ms(150);
        }
    }
    select_lcd();
    lcd_clear();
    unselect_lcd();

#ifdef STENO_STROKE_DISPLAY
    select_lcd();
    lcd_stenotype_frame();
    lcd_stenotype_update(0);
    unselect_lcd();
#endif
}
