/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --font /home/steven/.local/share/fonts/iosevka-term-regular.ttf --format lvgl --size 16 --bpp 1 -r 0x20-0x7F -o iosevka_16
 ******************************************************************************/

#include "lvgl.h"

#ifndef IOSEVKA_16
#define IOSEVKA_16 1
#endif

#if IOSEVKA_16

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0x55, 0x55, 0xf,

    /* U+0022 "\"" */
    0x99, 0x99, 0x90,

    /* U+0023 "#" */
    0x24, 0x48, 0x97, 0xf2, 0x44, 0x89, 0x12, 0xfe,
    0x48, 0x91, 0x22, 0x40,

    /* U+0024 "$" */
    0x10, 0x20, 0x43, 0xed, 0x72, 0x64, 0x38, 0x38,
    0x38, 0x4c, 0x9d, 0x6f, 0x84, 0x8,

    /* U+0025 "%" */
    0xf3, 0x2e, 0x54, 0xa9, 0x5e, 0x3, 0x95, 0x6a,
    0x95, 0x2e, 0x70,

    /* U+0026 "&" */
    0x30, 0x91, 0x22, 0x47, 0x6, 0x1d, 0x6a, 0x9d,
    0x1b, 0x33, 0xe0,

    /* U+0027 "'" */
    0xf8,

    /* U+0028 "(" */
    0x11, 0x98, 0x84, 0x42, 0x10, 0x84, 0x21, 0x4,
    0x20, 0x86, 0x0,

    /* U+0029 ")" */
    0x43, 0xc, 0x21, 0x4, 0x21, 0x8, 0x42, 0x11,
    0x8, 0x8c, 0x0,

    /* U+002A "*" */
    0x10, 0x23, 0x59, 0xc2, 0x8d, 0x80, 0x0,

    /* U+002B "+" */
    0x10, 0x4f, 0xc4, 0x10, 0x40,

    /* U+002C "," */
    0xfd, 0xbd, 0x0,

    /* U+002D "-" */
    0xfc,

    /* U+002E "." */
    0xfc,

    /* U+002F "/" */
    0x4, 0x30, 0x82, 0x18, 0x41, 0xc, 0x30, 0x82,
    0x18, 0x41, 0xc, 0x20,

    /* U+0030 "0" */
    0x7b, 0x38, 0x61, 0x8e, 0x7e, 0x71, 0x86, 0x1c,
    0xde,

    /* U+0031 "1" */
    0x30, 0xe1, 0x40, 0x81, 0x2, 0x4, 0x8, 0x10,
    0x20, 0x43, 0xf0,

    /* U+0032 "2" */
    0x7b, 0x38, 0x41, 0x4, 0x21, 0x98, 0x43, 0x8,
    0x3f,

    /* U+0033 "3" */
    0x79, 0x1c, 0x41, 0xc, 0xc0, 0x81, 0x6, 0x1c,
    0xde,

    /* U+0034 "4" */
    0x18, 0x63, 0x8a, 0x69, 0x2c, 0xbf, 0x8, 0x20,
    0x82,

    /* U+0035 "5" */
    0xfe, 0x8, 0x20, 0xfb, 0x28, 0x41, 0x6, 0x1c,
    0x9e,

    /* U+0036 "6" */
    0x10, 0xc2, 0x10, 0x7b, 0x28, 0x61, 0x86, 0x14,
    0x9e,

    /* U+0037 "7" */
    0xfc, 0x10, 0xc2, 0x8, 0x61, 0x4, 0x30, 0x82,
    0x18,

    /* U+0038 "8" */
    0x7a, 0x38, 0x61, 0xc9, 0xc7, 0xb3, 0x86, 0x1c,
    0xde,

    /* U+0039 "9" */
    0x7b, 0x28, 0x61, 0x87, 0x37, 0xc2, 0x18, 0x43,
    0x8,

    /* U+003A ":" */
    0xfc, 0x0, 0x7, 0xe0,

    /* U+003B ";" */
    0xfc, 0x0, 0x3f, 0x6f, 0x40,

    /* U+003C "<" */
    0xc, 0xee, 0x30, 0x70, 0x70, 0x40,

    /* U+003D "=" */
    0xfc, 0x0, 0x3f,

    /* U+003E ">" */
    0xc1, 0xc1, 0xc3, 0x3b, 0x88, 0x0,

    /* U+003F "?" */
    0xf0, 0xc2, 0x10, 0x98, 0x84, 0x0, 0x18, 0xc0,

    /* U+0040 "@" */
    0x7b, 0x38, 0x67, 0x96, 0x59, 0x65, 0x96, 0x59,
    0xf0, 0x7c,

    /* U+0041 "A" */
    0x30, 0xc7, 0x92, 0x49, 0x24, 0xa1, 0xfe, 0x18,
    0x61,

    /* U+0042 "B" */
    0xfa, 0x38, 0x61, 0x8f, 0xe8, 0xe1, 0x86, 0x18,
    0xfe,

    /* U+0043 "C" */
    0x79, 0x8a, 0x14, 0x8, 0x10, 0x20, 0x40, 0x81,
    0xb, 0x13, 0xc0,

    /* U+0044 "D" */
    0xfa, 0x38, 0x61, 0x86, 0x18, 0x61, 0x86, 0x18,
    0xfe,

    /* U+0045 "E" */
    0xfc, 0x21, 0x8, 0x7a, 0x10, 0x84, 0x21, 0xf0,

    /* U+0046 "F" */
    0xfc, 0x21, 0x8, 0x7a, 0x10, 0x84, 0x21, 0x0,

    /* U+0047 "G" */
    0x7b, 0x18, 0x60, 0x82, 0x78, 0x61, 0x86, 0x1c,
    0x5e,

    /* U+0048 "H" */
    0x86, 0x18, 0x61, 0x87, 0xf8, 0x61, 0x86, 0x18,
    0x61,

    /* U+0049 "I" */
    0xfc, 0x82, 0x8, 0x20, 0x82, 0x8, 0x20, 0x82,
    0x3f,

    /* U+004A "J" */
    0x1c, 0x10, 0x41, 0x4, 0x10, 0x41, 0x7, 0x14,
    0xce,

    /* U+004B "K" */
    0x8a, 0x69, 0x2c, 0xa3, 0xc, 0x28, 0xb2, 0x49,
    0xa2,

    /* U+004C "L" */
    0x84, 0x21, 0x8, 0x42, 0x10, 0x84, 0x21, 0xf0,

    /* U+004D "M" */
    0x87, 0x1c, 0xf3, 0xce, 0xdb, 0x6d, 0x86, 0x18,
    0x61,

    /* U+004E "N" */
    0xc7, 0x1c, 0x79, 0xa6, 0x99, 0x65, 0x9e, 0x38,
    0xe3,

    /* U+004F "O" */
    0x7b, 0x38, 0x61, 0x86, 0x18, 0x61, 0x86, 0x1c,
    0xde,

    /* U+0050 "P" */
    0xfa, 0x38, 0x61, 0x86, 0x3f, 0xa0, 0x82, 0x8,
    0x20,

    /* U+0051 "Q" */
    0x7b, 0x38, 0x61, 0x86, 0x18, 0x61, 0x86, 0x18,
    0x5e, 0x30, 0x41, 0xc0,

    /* U+0052 "R" */
    0xfa, 0x38, 0x61, 0x86, 0x3f, 0xa6, 0x8a, 0x28,
    0x61,

    /* U+0053 "S" */
    0x7b, 0x38, 0x60, 0xc1, 0x81, 0x83, 0x6, 0x18,
    0xde,

    /* U+0054 "T" */
    0xfc, 0x41, 0x4, 0x10, 0x41, 0x4, 0x10, 0x41,
    0x4,

    /* U+0055 "U" */
    0x86, 0x18, 0x61, 0x86, 0x18, 0x61, 0x86, 0x1c,
    0xde,

    /* U+0056 "V" */
    0x86, 0x18, 0x61, 0x85, 0x24, 0x92, 0x49, 0xe3,
    0xc,

    /* U+0057 "W" */
    0x86, 0x18, 0x61, 0x86, 0xdb, 0x6d, 0xb9, 0x24,
    0x92,

    /* U+0058 "X" */
    0x86, 0x18, 0xd2, 0x78, 0xc3, 0x1e, 0x4a, 0x38,
    0x61,

    /* U+0059 "Y" */
    0x83, 0x6, 0xa, 0x26, 0xc5, 0xe, 0x8, 0x10,
    0x20, 0x40, 0x80,

    /* U+005A "Z" */
    0xfc, 0x10, 0xc2, 0x18, 0x42, 0x18, 0x43, 0x8,
    0x3f,

    /* U+005B "[" */
    0xfc, 0x21, 0x8, 0x42, 0x10, 0x84, 0x21, 0x8,
    0x42, 0x1f,

    /* U+005C "\\" */
    0x83, 0x4, 0x10, 0x60, 0x82, 0xc, 0x30, 0x41,
    0x6, 0x8, 0x20, 0xc1,

    /* U+005D "]" */
    0xf8, 0x42, 0x10, 0x84, 0x21, 0x8, 0x42, 0x10,
    0x84, 0x3f,

    /* U+005E "^" */
    0x31, 0xec, 0xe1,

    /* U+005F "_" */
    0xfc,

    /* U+0060 "`" */
    0x46, 0x31,

    /* U+0061 "a" */
    0x7b, 0x38, 0x41, 0x7f, 0x18, 0x63, 0x74,

    /* U+0062 "b" */
    0x82, 0x8, 0x2e, 0xce, 0x18, 0x61, 0x86, 0x1c,
    0xee,

    /* U+0063 "c" */
    0x79, 0x9a, 0x14, 0x8, 0x10, 0x21, 0x66, 0x78,

    /* U+0064 "d" */
    0x4, 0x10, 0x5d, 0xce, 0x18, 0x61, 0x86, 0x1c,
    0xdd,

    /* U+0065 "e" */
    0x7b, 0x38, 0x61, 0xfe, 0x8, 0x31, 0x78,

    /* U+0066 "f" */
    0x1c, 0x48, 0x81, 0xf, 0xc4, 0x8, 0x10, 0x20,
    0x40, 0x81, 0x0,

    /* U+0067 "g" */
    0x77, 0x38, 0x61, 0x86, 0x18, 0x73, 0x74, 0x1c,
    0xde,

    /* U+0068 "h" */
    0x82, 0x8, 0x2e, 0xce, 0x18, 0x61, 0x86, 0x18,
    0x61,

    /* U+0069 "i" */
    0x30, 0xc0, 0x38, 0x20, 0x82, 0x8, 0x20, 0x82,
    0x3f,

    /* U+006A "j" */
    0xc, 0x30, 0xf, 0x4, 0x10, 0x41, 0x4, 0x10,
    0x41, 0xc5, 0x33, 0x80,

    /* U+006B "k" */
    0x82, 0x8, 0x22, 0x92, 0xce, 0x30, 0xe2, 0xc9,
    0xa2,

    /* U+006C "l" */
    0xe0, 0x82, 0x8, 0x20, 0x82, 0x8, 0x20, 0x82,
    0x3f,

    /* U+006D "m" */
    0xfd, 0x26, 0x4c, 0x99, 0x32, 0x64, 0xc9, 0x92,

    /* U+006E "n" */
    0xbb, 0x38, 0x61, 0x86, 0x18, 0x61, 0x84,

    /* U+006F "o" */
    0x7b, 0x38, 0x61, 0x86, 0x18, 0x73, 0x78,

    /* U+0070 "p" */
    0xbb, 0x38, 0x61, 0x86, 0x18, 0x73, 0xba, 0x8,
    0x20,

    /* U+0071 "q" */
    0x77, 0x38, 0x61, 0x86, 0x18, 0x73, 0x74, 0x10,
    0x41,

    /* U+0072 "r" */
    0xb6, 0x61, 0x8, 0x42, 0x10, 0x80,

    /* U+0073 "s" */
    0x7a, 0x38, 0x30, 0x78, 0x30, 0x61, 0x78,

    /* U+0074 "t" */
    0x20, 0x40, 0x87, 0xe2, 0x4, 0x8, 0x10, 0x20,
    0x44, 0x88, 0xe0,

    /* U+0075 "u" */
    0x86, 0x18, 0x61, 0x86, 0x18, 0x73, 0x74,

    /* U+0076 "v" */
    0x86, 0x18, 0x61, 0x49, 0x24, 0x8c, 0x30,

    /* U+0077 "w" */
    0x86, 0x18, 0x61, 0xb6, 0xdb, 0x92, 0x48,

    /* U+0078 "x" */
    0x86, 0x14, 0x9e, 0x31, 0xe4, 0xa1, 0x84,

    /* U+0079 "y" */
    0x86, 0x18, 0x72, 0x49, 0x63, 0xc, 0x30, 0x86,
    0x18,

    /* U+007A "z" */
    0xfc, 0x30, 0xc6, 0x31, 0x8c, 0x20, 0xfc,

    /* U+007B "{" */
    0x1c, 0xc2, 0x8, 0x10, 0x43, 0x38, 0x30, 0x41,
    0xc, 0x20, 0x83, 0x3,

    /* U+007C "|" */
    0xff, 0xff,

    /* U+007D "}" */
    0xe0, 0xc1, 0x4, 0x20, 0x83, 0x7, 0x30, 0x82,
    0xc, 0x10, 0x43, 0x30,

    /* U+007E "~" */
    0x0, 0x72, 0x5a, 0xcc
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 128, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 128, .box_w = 2, .box_h = 12, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 128, .box_w = 4, .box_h = 5, .ofs_x = 2, .ofs_y = 8},
    {.bitmap_index = 7, .adv_w = 128, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 19, .adv_w = 128, .box_w = 7, .box_h = 16, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 33, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 44, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 55, .adv_w = 128, .box_w = 1, .box_h = 5, .ofs_x = 3, .ofs_y = 8},
    {.bitmap_index = 56, .adv_w = 128, .box_w = 5, .box_h = 17, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 67, .adv_w = 128, .box_w = 5, .box_h = 17, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 78, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 85, .adv_w = 128, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 90, .adv_w = 128, .box_w = 3, .box_h = 6, .ofs_x = 3, .ofs_y = -4},
    {.bitmap_index = 93, .adv_w = 128, .box_w = 6, .box_h = 1, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 94, .adv_w = 128, .box_w = 3, .box_h = 2, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 95, .adv_w = 128, .box_w = 6, .box_h = 16, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 107, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 116, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 127, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 136, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 145, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 154, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 172, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 181, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 190, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 199, .adv_w = 128, .box_w = 3, .box_h = 9, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 203, .adv_w = 128, .box_w = 3, .box_h = 12, .ofs_x = 3, .ofs_y = -3},
    {.bitmap_index = 208, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 214, .adv_w = 128, .box_w = 6, .box_h = 4, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 217, .adv_w = 128, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 223, .adv_w = 128, .box_w = 5, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 231, .adv_w = 128, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 241, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 250, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 259, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 270, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 279, .adv_w = 128, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 287, .adv_w = 128, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 295, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 304, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 313, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 322, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 331, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 340, .adv_w = 128, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 348, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 357, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 366, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 375, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 384, .adv_w = 128, .box_w = 6, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 396, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 405, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 414, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 423, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 432, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 441, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 450, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 459, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 470, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 479, .adv_w = 128, .box_w = 5, .box_h = 16, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 489, .adv_w = 128, .box_w = 6, .box_h = 16, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 501, .adv_w = 128, .box_w = 5, .box_h = 16, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 511, .adv_w = 128, .box_w = 6, .box_h = 4, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 514, .adv_w = 128, .box_w = 6, .box_h = 1, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 515, .adv_w = 128, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 9},
    {.bitmap_index = 517, .adv_w = 128, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 524, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 533, .adv_w = 128, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 541, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 550, .adv_w = 128, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 557, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 568, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 577, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 586, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 595, .adv_w = 128, .box_w = 6, .box_h = 15, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 607, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 616, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 625, .adv_w = 128, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 633, .adv_w = 128, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 640, .adv_w = 128, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 647, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 656, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 665, .adv_w = 128, .box_w = 5, .box_h = 9, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 671, .adv_w = 128, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 678, .adv_w = 128, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 689, .adv_w = 128, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 696, .adv_w = 128, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 703, .adv_w = 128, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 710, .adv_w = 128, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 717, .adv_w = 128, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 726, .adv_w = 128, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 733, .adv_w = 128, .box_w = 6, .box_h = 16, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 745, .adv_w = 128, .box_w = 1, .box_h = 16, .ofs_x = 3, .ofs_y = -2},
    {.bitmap_index = 747, .adv_w = 128, .box_w = 6, .box_h = 16, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 759, .adv_w = 128, .box_w = 8, .box_h = 4, .ofs_x = 0, .ofs_y = 4}
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
const lv_font_t iosevka_16 = {
#else
lv_font_t iosevka_16 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 16,          /*The maximum line height required by the font*/
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



#endif /*#if IOSEVKA_16*/

