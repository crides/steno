/*******************************************************************************
 * Size: 12 px
 * Bpp: 1
 * Opts: --font /home/steven/.local/share/fonts/iosevka-term-regular.ttf --format lvgl --size 12 --bpp 1 -r 0x20-0x7F -o iosevka_12
 ******************************************************************************/

#include "lvgl.h"

#ifndef IOSEVKA_12
#define IOSEVKA_12 1
#endif

#if IOSEVKA_12

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0x55, 0x50, 0xf0,

    /* U+0022 "\"" */
    0xb6, 0xd0,

    /* U+0023 "#" */
    0x52, 0x95, 0xf5, 0x29, 0x5f, 0x52, 0x94,

    /* U+0024 "$" */
    0x21, 0x1d, 0x5a, 0x71, 0xc7, 0x2d, 0x5c, 0x42,
    0x0,

    /* U+0025 "%" */
    0xed, 0x6d, 0x6e, 0xd, 0x6b, 0x5c, 0xc0,

    /* U+0026 "&" */
    0x62, 0x49, 0x2c, 0x61, 0xaa, 0xa6, 0x99, 0xe0,

    /* U+0027 "'" */
    0xf0,

    /* U+0028 "(" */
    0x2, 0x4c, 0x88, 0x88, 0x88, 0x46, 0x30,

    /* U+0029 ")" */
    0x4, 0x23, 0x11, 0x11, 0x11, 0x26, 0xc0,

    /* U+002A "*" */
    0x21, 0x3e, 0xa5, 0x0,

    /* U+002B "+" */
    0x21, 0x3e, 0x42, 0x0,

    /* U+002C "," */
    0xf6, 0x0,

    /* U+002D "-" */
    0xf0,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0x11, 0x12, 0x22, 0x24, 0x44, 0x88, 0x80,

    /* U+0030 "0" */
    0x74, 0x63, 0x1b, 0xf6, 0x31, 0x8b, 0x80,

    /* U+0031 "1" */
    0x67, 0x8, 0x42, 0x10, 0x84, 0x27, 0xc0,

    /* U+0032 "2" */
    0x72, 0x42, 0x11, 0x99, 0x88, 0x47, 0xc0,

    /* U+0033 "3" */
    0x72, 0x42, 0x33, 0xc, 0x31, 0x4b, 0x80,

    /* U+0034 "4" */
    0x11, 0x8c, 0xa5, 0x4b, 0xe2, 0x10, 0x80,

    /* U+0035 "5" */
    0xfc, 0x21, 0xf, 0x44, 0x21, 0x8b, 0x80,

    /* U+0036 "6" */
    0x21, 0x10, 0x8f, 0x46, 0x31, 0x8b, 0x80,

    /* U+0037 "7" */
    0xf8, 0x42, 0x21, 0x8, 0x84, 0x21, 0x0,

    /* U+0038 "8" */
    0x74, 0x63, 0xb7, 0x3a, 0x31, 0x8b, 0x80,

    /* U+0039 "9" */
    0x74, 0x63, 0x18, 0xbc, 0x42, 0x21, 0x0,

    /* U+003A ":" */
    0xf0, 0x3c,

    /* U+003B ";" */
    0xf0, 0x3d, 0x80,

    /* U+003C "<" */
    0x1b, 0x20, 0xc1, 0x80,

    /* U+003D "=" */
    0xf0, 0xf0,

    /* U+003E ">" */
    0xc1, 0x82, 0x6c, 0x0,

    /* U+003F "?" */
    0xe1, 0x11, 0x16, 0x40, 0xcc,

    /* U+0040 "@" */
    0x74, 0x63, 0x7a, 0xd6, 0xb5, 0xbc, 0x1e,

    /* U+0041 "A" */
    0x22, 0x94, 0xa5, 0x6e, 0x3f, 0x8c, 0x40,

    /* U+0042 "B" */
    0xe4, 0xa5, 0x29, 0x72, 0x71, 0x8f, 0x80,

    /* U+0043 "C" */
    0x74, 0xe1, 0x8, 0x42, 0x10, 0x9b, 0x80,

    /* U+0044 "D" */
    0xf4, 0xe3, 0x18, 0xc6, 0x31, 0x9f, 0x80,

    /* U+0045 "E" */
    0xf8, 0x88, 0xf8, 0x88, 0x8f,

    /* U+0046 "F" */
    0xf8, 0x88, 0xf8, 0x88, 0x88,

    /* U+0047 "G" */
    0x74, 0x63, 0x8, 0x4e, 0x31, 0x8b, 0x80,

    /* U+0048 "H" */
    0x8c, 0x63, 0x18, 0xfe, 0x31, 0x8c, 0x40,

    /* U+0049 "I" */
    0xf9, 0x8, 0x42, 0x10, 0x84, 0x27, 0xc0,

    /* U+004A "J" */
    0x38, 0x42, 0x10, 0x84, 0x31, 0x4b, 0x80,

    /* U+004B "K" */
    0x94, 0xa9, 0x4c, 0x62, 0x94, 0x94, 0x80,

    /* U+004C "L" */
    0x88, 0x88, 0x88, 0x88, 0x8f,

    /* U+004D "M" */
    0x8c, 0x77, 0xbe, 0xf6, 0xb1, 0x8c, 0x40,

    /* U+004E "N" */
    0x8e, 0x73, 0x9a, 0xd6, 0xb3, 0x9c, 0x40,

    /* U+004F "O" */
    0x74, 0x63, 0x18, 0xc6, 0x31, 0x8b, 0x80,

    /* U+0050 "P" */
    0xf4, 0xe3, 0x19, 0xfa, 0x10, 0x84, 0x0,

    /* U+0051 "Q" */
    0x74, 0x63, 0x18, 0xc6, 0x31, 0x8b, 0x88, 0x30,

    /* U+0052 "R" */
    0xf4, 0xe3, 0x19, 0xfa, 0x52, 0x8c, 0x40,

    /* U+0053 "S" */
    0x74, 0x61, 0x6, 0x18, 0x21, 0x8b, 0x80,

    /* U+0054 "T" */
    0xf9, 0x8, 0x42, 0x10, 0x84, 0x21, 0x0,

    /* U+0055 "U" */
    0x8c, 0x63, 0x18, 0xc6, 0x31, 0x8b, 0x80,

    /* U+0056 "V" */
    0x8c, 0x63, 0x18, 0xa9, 0x4a, 0x51, 0x0,

    /* U+0057 "W" */
    0x8c, 0x63, 0x1a, 0xf7, 0xbe, 0x52, 0x80,

    /* U+0058 "X" */
    0x8c, 0x76, 0xa2, 0x11, 0x5b, 0x8c, 0x40,

    /* U+0059 "Y" */
    0x8c, 0x62, 0xa5, 0x10, 0x84, 0x21, 0x0,

    /* U+005A "Z" */
    0xf8, 0x84, 0x22, 0x21, 0x10, 0x87, 0xc0,

    /* U+005B "[" */
    0xf8, 0x88, 0x88, 0x88, 0x88, 0x88, 0xf0,

    /* U+005C "\\" */
    0x88, 0x84, 0x44, 0x42, 0x22, 0x11, 0x10,

    /* U+005D "]" */
    0xf1, 0x11, 0x11, 0x11, 0x11, 0x11, 0xf0,

    /* U+005E "^" */
    0x6f, 0x90,

    /* U+005F "_" */
    0xf0,

    /* U+0060 "`" */
    0x88, 0x80,

    /* U+0061 "a" */
    0x74, 0x42, 0xf8, 0xc7, 0xe0,

    /* U+0062 "b" */
    0x84, 0x21, 0x6c, 0xc6, 0x31, 0x8f, 0x80,

    /* U+0063 "c" */
    0x74, 0xe1, 0x8, 0x4d, 0xc0,

    /* U+0064 "d" */
    0x8, 0x42, 0xd9, 0xc6, 0x31, 0x8b, 0xc0,

    /* U+0065 "e" */
    0x74, 0x63, 0xf8, 0x45, 0xc0,

    /* U+0066 "f" */
    0x75, 0x4f, 0x44, 0x44, 0x44,

    /* U+0067 "g" */
    0x6c, 0xe3, 0x18, 0xc5, 0xe1, 0x8b, 0x80,

    /* U+0068 "h" */
    0x84, 0x21, 0x6c, 0xc6, 0x31, 0x8c, 0x40,

    /* U+0069 "i" */
    0x60, 0x1, 0xc2, 0x10, 0x84, 0x27, 0xc0,

    /* U+006A "j" */
    0x10, 0x7, 0x11, 0x11, 0x11, 0x19, 0x60,

    /* U+006B "k" */
    0x84, 0x21, 0x2a, 0x73, 0x1c, 0xa4, 0x80,

    /* U+006C "l" */
    0xe1, 0x8, 0x42, 0x10, 0x84, 0x27, 0xc0,

    /* U+006D "m" */
    0xfd, 0x6b, 0x5a, 0xd6, 0xa0,

    /* U+006E "n" */
    0xb6, 0x63, 0x18, 0xc6, 0x20,

    /* U+006F "o" */
    0x74, 0x63, 0x18, 0xc5, 0xc0,

    /* U+0070 "p" */
    0xb6, 0x63, 0x18, 0xc7, 0xd0, 0x84, 0x0,

    /* U+0071 "q" */
    0x6c, 0xe3, 0x18, 0xc5, 0xe1, 0x8, 0x40,

    /* U+0072 "r" */
    0xbd, 0x88, 0x88, 0x80,

    /* U+0073 "s" */
    0x74, 0xa0, 0xe1, 0x49, 0xc0,

    /* U+0074 "t" */
    0x42, 0x11, 0xe4, 0x21, 0x8, 0x49, 0x80,

    /* U+0075 "u" */
    0x8c, 0x63, 0x18, 0xc5, 0xe0,

    /* U+0076 "v" */
    0x8c, 0x62, 0xa5, 0x28, 0x80,

    /* U+0077 "w" */
    0x8c, 0x63, 0x5e, 0xf9, 0x40,

    /* U+0078 "x" */
    0x94, 0x9c, 0x47, 0x4a, 0x20,

    /* U+0079 "y" */
    0x8c, 0x62, 0xa5, 0x30, 0x84, 0x42, 0x0,

    /* U+007A "z" */
    0xf0, 0x84, 0x44, 0x43, 0xe0,

    /* U+007B "{" */
    0x34, 0x46, 0x22, 0xc2, 0x26, 0x44, 0x30,

    /* U+007C "|" */
    0xff, 0xf8,

    /* U+007D "}" */
    0xc2, 0x26, 0x44, 0x34, 0x46, 0x22, 0xc0,

    /* U+007E "~" */
    0x65, 0xe9, 0x80
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 96, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 96, .box_w = 2, .box_h = 10, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 96, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 6, .adv_w = 96, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 13, .adv_w = 96, .box_w = 5, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 22, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 29, .adv_w = 96, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 37, .adv_w = 96, .box_w = 1, .box_h = 4, .ofs_x = 3, .ofs_y = 6},
    {.bitmap_index = 38, .adv_w = 96, .box_w = 4, .box_h = 13, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 45, .adv_w = 96, .box_w = 4, .box_h = 13, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 52, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 56, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 60, .adv_w = 96, .box_w = 2, .box_h = 5, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 62, .adv_w = 96, .box_w = 4, .box_h = 1, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 63, .adv_w = 96, .box_w = 2, .box_h = 2, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 64, .adv_w = 96, .box_w = 4, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 71, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 78, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 85, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 92, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 99, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 106, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 113, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 120, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 127, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 134, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 141, .adv_w = 96, .box_w = 2, .box_h = 7, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 143, .adv_w = 96, .box_w = 2, .box_h = 10, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 146, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 150, .adv_w = 96, .box_w = 4, .box_h = 3, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 152, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 156, .adv_w = 96, .box_w = 4, .box_h = 10, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 161, .adv_w = 96, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 168, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 175, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 182, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 189, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 196, .adv_w = 96, .box_w = 4, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 201, .adv_w = 96, .box_w = 4, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 206, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 213, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 220, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 227, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 234, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 241, .adv_w = 96, .box_w = 4, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 246, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 253, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 260, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 267, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 274, .adv_w = 96, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 282, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 289, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 296, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 303, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 310, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 317, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 324, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 331, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 338, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 345, .adv_w = 96, .box_w = 4, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 352, .adv_w = 96, .box_w = 4, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 359, .adv_w = 96, .box_w = 4, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 366, .adv_w = 96, .box_w = 4, .box_h = 3, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 368, .adv_w = 96, .box_w = 4, .box_h = 1, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 369, .adv_w = 96, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 371, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 376, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 383, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 388, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 395, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 400, .adv_w = 96, .box_w = 4, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 405, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 412, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 419, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 426, .adv_w = 96, .box_w = 4, .box_h = 13, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 433, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 440, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 447, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 452, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 457, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 462, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 469, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 476, .adv_w = 96, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 480, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 485, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 492, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 497, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 502, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 507, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 512, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 519, .adv_w = 96, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 524, .adv_w = 96, .box_w = 4, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 531, .adv_w = 96, .box_w = 1, .box_h = 13, .ofs_x = 3, .ofs_y = -2},
    {.bitmap_index = 533, .adv_w = 96, .box_w = 4, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 540, .adv_w = 96, .box_w = 6, .box_h = 3, .ofs_x = 0, .ofs_y = 3}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t iosevka_12 = {
#else
lv_font_t iosevka_12 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 12,          /*The maximum line height required by the font*/
    .base_line = 1,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0)
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if IOSEVKA_12*/

