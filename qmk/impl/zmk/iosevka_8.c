/*******************************************************************************
 * Size: 8 px
 * Bpp: 1
 * Opts: --font /home/steven/.local/share/fonts/iosevka-term-regular.ttf --format lvgl --size 8 --bpp 1 -r 0x20-0x7F -o iosevka_8
 ******************************************************************************/

#include "lvgl.h"

#ifndef IOSEVKA_8
#define IOSEVKA_8 1
#endif

#if IOSEVKA_8

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0x55, 0xc0,

    /* U+0022 "\"" */
    0xf0,

    /* U+0023 "#" */
    0x6f, 0x6f, 0x66,

    /* U+0024 "$" */
    0x5f, 0x66, 0xfa,

    /* U+0025 "%" */
    0xdc, 0x3b, 0xb0,

    /* U+0026 "&" */
    0x6a, 0x5b, 0x70,

    /* U+0027 "'" */
    0xc0,

    /* U+0028 "(" */
    0x52, 0x48, 0x90,

    /* U+0029 ")" */
    0x44, 0x92, 0xb0,

    /* U+002A "*" */
    0x5e, 0x80,

    /* U+002B "+" */
    0x5d, 0x0,

    /* U+002C "," */
    0xc0,

    /* U+002D "-" */
    0xf0,

    /* U+002E "." */
    0xc0,

    /* U+002F "/" */
    0x22, 0x24, 0x44, 0x80,

    /* U+0030 "0" */
    0xf7, 0xde,

    /* U+0031 "1" */
    0xc9, 0x2e,

    /* U+0032 "2" */
    0xe5, 0x4e,

    /* U+0033 "3" */
    0xe5, 0x1e,

    /* U+0034 "4" */
    0x26, 0xaf, 0x20,

    /* U+0035 "5" */
    0xfa, 0xde,

    /* U+0036 "6" */
    0x4a, 0xde,

    /* U+0037 "7" */
    0xe5, 0x28,

    /* U+0038 "8" */
    0xf5, 0x5e,

    /* U+0039 "9" */
    0xf7, 0xa4,

    /* U+003A ":" */
    0xc3,

    /* U+003B ";" */
    0xc1, 0x40,

    /* U+003C "<" */
    0x71, 0x80,

    /* U+003D "=" */
    0xff,

    /* U+003E ">" */
    0xc7, 0x0,

    /* U+003F "?" */
    0xe5, 0xc,

    /* U+0040 "@" */
    0xff, 0xff, 0xc0,

    /* U+0041 "A" */
    0x5a, 0xfa,

    /* U+0042 "B" */
    0xf7, 0x5e,

    /* U+0043 "C" */
    0xf2, 0x4e,

    /* U+0044 "D" */
    0xf6, 0xde,

    /* U+0045 "E" */
    0xf3, 0xce,

    /* U+0046 "F" */
    0xf3, 0xc8,

    /* U+0047 "G" */
    0xf2, 0xde,

    /* U+0048 "H" */
    0xb7, 0xda,

    /* U+0049 "I" */
    0xe9, 0x2e,

    /* U+004A "J" */
    0x64, 0x9e,

    /* U+004B "K" */
    0xbb, 0x6a,

    /* U+004C "L" */
    0x92, 0x4e,

    /* U+004D "M" */
    0xb6, 0xfa,

    /* U+004E "N" */
    0xb7, 0xda,

    /* U+004F "O" */
    0xf6, 0xde,

    /* U+0050 "P" */
    0xf7, 0xc8,

    /* U+0051 "Q" */
    0xf6, 0xda, 0x88,

    /* U+0052 "R" */
    0xf7, 0xda,

    /* U+0053 "S" */
    0xf1, 0x1e,

    /* U+0054 "T" */
    0xe9, 0x24,

    /* U+0055 "U" */
    0xb6, 0xde,

    /* U+0056 "V" */
    0xb6, 0xe4,

    /* U+0057 "W" */
    0xb7, 0xf0,

    /* U+0058 "X" */
    0xb5, 0x5a,

    /* U+0059 "Y" */
    0xb5, 0x24,

    /* U+005A "Z" */
    0xe5, 0x4e,

    /* U+005B "[" */
    0xf2, 0x49, 0x27,

    /* U+005C "\\" */
    0x84, 0x44, 0x22, 0x20,

    /* U+005D "]" */
    0xe4, 0x92, 0x4f,

    /* U+005E "^" */
    0x60,

    /* U+005F "_" */
    0xf0,

    /* U+0060 "`" */
    0xa0,

    /* U+0061 "a" */
    0xee, 0xf0,

    /* U+0062 "b" */
    0x9e, 0xde,

    /* U+0063 "c" */
    0xf2, 0x70,

    /* U+0064 "d" */
    0x3e, 0xde,

    /* U+0065 "e" */
    0xfe, 0x70,

    /* U+0066 "f" */
    0x7e, 0x44, 0x40,

    /* U+0067 "g" */
    0xf6, 0xfb, 0xc0,

    /* U+0068 "h" */
    0x9e, 0xda,

    /* U+0069 "i" */
    0x99, 0x2e,

    /* U+006A "j" */
    0x2c, 0x92, 0x78,

    /* U+006B "k" */
    0x97, 0x6a,

    /* U+006C "l" */
    0xc9, 0x2e,

    /* U+006D "m" */
    0xff, 0xf0,

    /* U+006E "n" */
    0xf6, 0xd0,

    /* U+006F "o" */
    0xf6, 0xf0,

    /* U+0070 "p" */
    0xf6, 0xf9, 0x0,

    /* U+0071 "q" */
    0xf6, 0xf2, 0x40,

    /* U+0072 "r" */
    0xf2, 0x40,

    /* U+0073 "s" */
    0x68, 0x66,

    /* U+0074 "t" */
    0x4e, 0x44, 0x70,

    /* U+0075 "u" */
    0xb6, 0xf0,

    /* U+0076 "v" */
    0xb7, 0x60,

    /* U+0077 "w" */
    0xb7, 0xe0,

    /* U+0078 "x" */
    0xa6, 0x6a,

    /* U+0079 "y" */
    0xb7, 0x24,

    /* U+007A "z" */
    0xe2, 0x47,

    /* U+007B "{" */
    0x6a, 0xa9,

    /* U+007C "|" */
    0xfe,

    /* U+007D "}" */
    0x9a, 0x66,

    /* U+007E "~" */
    0xe
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 64, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 64, .box_w = 2, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 3, .adv_w = 64, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 4, .adv_w = 64, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 7, .adv_w = 64, .box_w = 3, .box_h = 8, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 10, .adv_w = 64, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 13, .adv_w = 64, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 16, .adv_w = 64, .box_w = 1, .box_h = 2, .ofs_x = 2, .ofs_y = 3},
    {.bitmap_index = 17, .adv_w = 64, .box_w = 3, .box_h = 7, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 20, .adv_w = 64, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 23, .adv_w = 64, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 25, .adv_w = 64, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 27, .adv_w = 64, .box_w = 1, .box_h = 3, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 28, .adv_w = 64, .box_w = 4, .box_h = 1, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 29, .adv_w = 64, .box_w = 2, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 30, .adv_w = 64, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 34, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 36, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 38, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 40, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 42, .adv_w = 64, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 45, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 47, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 49, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 51, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 53, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 55, .adv_w = 64, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 56, .adv_w = 64, .box_w = 2, .box_h = 5, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 58, .adv_w = 64, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 60, .adv_w = 64, .box_w = 4, .box_h = 2, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 61, .adv_w = 64, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 63, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 65, .adv_w = 64, .box_w = 3, .box_h = 6, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 68, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 70, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 72, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 74, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 76, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 78, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 80, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 82, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 84, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 86, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 88, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 90, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 92, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 94, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 96, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 98, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 100, .adv_w = 64, .box_w = 3, .box_h = 7, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 103, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 105, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 107, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 109, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 111, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 113, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 115, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 117, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 119, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 121, .adv_w = 64, .box_w = 3, .box_h = 8, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 124, .adv_w = 64, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 128, .adv_w = 64, .box_w = 3, .box_h = 8, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 131, .adv_w = 64, .box_w = 4, .box_h = 2, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 132, .adv_w = 64, .box_w = 4, .box_h = 1, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 133, .adv_w = 64, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 134, .adv_w = 64, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 136, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 138, .adv_w = 64, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 140, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 142, .adv_w = 64, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 144, .adv_w = 64, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 147, .adv_w = 64, .box_w = 3, .box_h = 6, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 150, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 152, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 154, .adv_w = 64, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 157, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 159, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 161, .adv_w = 64, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 64, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 165, .adv_w = 64, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 167, .adv_w = 64, .box_w = 3, .box_h = 6, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 170, .adv_w = 64, .box_w = 3, .box_h = 6, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 173, .adv_w = 64, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 175, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 177, .adv_w = 64, .box_w = 4, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 180, .adv_w = 64, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 182, .adv_w = 64, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 184, .adv_w = 64, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 186, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 188, .adv_w = 64, .box_w = 3, .box_h = 5, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 190, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 192, .adv_w = 64, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 194, .adv_w = 64, .box_w = 1, .box_h = 7, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 195, .adv_w = 64, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 197, .adv_w = 64, .box_w = 4, .box_h = 2, .ofs_x = 0, .ofs_y = 2}
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
const lv_font_t iosevka_8 = {
#else
lv_font_t iosevka_8 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 9,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0)
    .underline_position = 0,
    .underline_thickness = 0,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if IOSEVKA_8*/

