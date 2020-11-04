#pragma once

#include <stdbool.h>
#include "steno.h"

#define MAX_COLLISIONS 8
#define SEARCH_NODES_SIZE 8
#define FNV_SEED 0x811c9dc5
#define FNV_FACTOR 0x01000193

// Caps for the current entry
typedef enum {
    // all lower case
    CAPS_LOWER = 0,
    // Capitalized; only first charater is upper
    CAPS_CAP = 1,
    // ALL CAPS
    CAPS_UPPER = 2,
} caps_t;

typedef struct __attribute__((packed)) {
    uint8_t space_prev : 1;
    uint8_t space_after : 1;
    uint8_t glue : 1;
    uint8_t present : 1;
} attr_t;

extern uint32_t last_entry_ptr;
extern uint8_t entry_buf[128];

bool stroke_to_string(uint32_t stroke, char *buf, uint8_t *len);
uint32_t qmk_chord_to_stroke(uint8_t chord[6]);
uint8_t last_entry_len(void);
// Both returns implicitly in `last_entry_ptr`
void find_strokes(uint8_t *strokes, uint8_t len);
void search_entry(uint8_t h_ind);
