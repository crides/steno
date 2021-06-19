#pragma once

#include <stdbool.h>
#include "steno.h"

#define MAX_COLLISIONS 8
#define SEARCH_NODES_SIZE 8
#define FNV_SEED 0x811c9dc5
#define FNV_FACTOR 0x01000193

#define STROKE_SIZE 3
#define BUCKET_SIZE 4
#define MAX_STROKE_NUM 14
#define U24_FROM_PTR_LE(p) (((uint32_t)(p)[2] << 16) | ((uint32_t)(p)[1] << 8) | ((uint32_t)(p)[0]))
#define STROKE_FROM_PTR(p) U24_FROM_PTR_LE(p)

#define BUCKET_START 0
#define KVPAIR_BLOCK_START  0x400000
#define FREEMAP_START       0xF00000
#define SCRATCH_START       0xF22000
#define ORTHOGRAPHY_START   0xF30000
#define FLOG_START          0xF80000
#define STORE_END          0x1000000

#define BUCKET_GET_ENTRY_LEN(e) ((e >> 24) & 0xFF)
#define BUCKET_GET_STROKES_LEN(e) (e & 0x0F)
#define BUCKET_GET_ADDR(e) ((e & 0xFFFFF0) + KVPAIR_BLOCK_START)
#define BUCKET_GET_ENTRY_PTR(e) (BUCKET_GET_ADDR(e) + STROKE_SIZE * BUCKET_GET_STROKES_LEN(e) + 1)

#define FREEMAP_LVL_0 FREEMAP_START
#define FREEMAP_LVL_1 ((1ul << 20) / 32 * 4 + FREEMAP_LVL_0)
#define FREEMAP_LVL_2 ((1ul << 20) / 32 / 32 * 4 + FREEMAP_LVL_1)
#define FREEMAP_LVL_3 ((1ul << 20) / 32 / 32 / 32 * 4 + FREEMAP_LVL_2)

// Caps for the current entry
typedef enum {
    // No change
    CAPS_NORMAL = 0,
    // Capitalized; only first charater is upper
    CAPS_CAP = 1,
    // ALL CAPS
    CAPS_UPPER = 2,
    // all lower case
    CAPS_LOWER = 3,
} caps_t;

typedef struct __attribute__((packed)) {
    uint8_t space_prev : 1;
    uint8_t space_after : 1;
    uint8_t glue : 1;
} attr_t;

typedef struct __attribute__((packed)) {
    uint8_t letter : 5;
    uint32_t offset;
} node_t;

typedef struct __attribute__((packed)) {
    uint8_t node_num : 5;
    uint32_t offset;
} header_t;

typedef struct __attribute__((packed)) {
    uint8_t word_len : 4;
    uint8_t suffix_len : 4;
    char word[13];
    char suffix[9];
    uint8_t use_suffix : 1;
    uint8_t use_word_chars : 3;
    uint8_t extra_text_len : 4;
    char extra_text[8];
} orthography_entry_t;

extern uint8_t kvpair_buf[128];

bool stroke_to_string(const uint32_t stroke, char *const buf, uint8_t *const ret_len);
uint32_t qmk_chord_to_stroke(const uint8_t chord[6]);
uint8_t last_entry_len(void);
uint32_t find_strokes(const uint8_t *strokes, const uint8_t len, const uint8_t free);
uint32_t search_entry(const uint8_t h_ind);
uint32_t freemap_req(const uint8_t block);
void print_strokes(const uint8_t *strokes, const uint8_t len);
void read_entry(const uint32_t bucket, uint8_t *buf);
