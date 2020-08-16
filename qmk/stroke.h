#pragma once

#include <stdbool.h>
#include "steno.h"

#define MAX_COLLISIONS 8
#define SEARCH_NODES_SIZE 8

typedef union __attribute__((packed)) {
    uint32_t raw : 24;
    struct {
        uint8_t b0;
        uint8_t b1;
        uint8_t b2;
    };
} u24;

typedef struct __attribute__((packed)) {
    uint32_t stroke : 24;
    uint32_t addr : 24;
} child_t;

// Caps for the current entry
typedef enum {
    // all lower case
    CAPS_LOWER = 0,
    // Capitalized; only first charater is upper
    CAPS_CAP = 1,
    // ALL CAPS
    CAPS_UPPER = 2,
} caps_t;

// Attributes for next entry
typedef enum {
    ATTR_CAPS_LOWER = 0,
    ATTR_CAPS_KEEP = 1,
    ATTR_CAPS_CAPS = 2,
    ATTR_CAPS_UPPER = 3,
} attr_caps_t;

typedef struct __attribute__((packed)) {
    attr_caps_t caps : 2;
    uint8_t space_prev : 1;
    uint8_t space_after : 1;
    uint8_t glue : 1;
    uint8_t present : 1;
    uint8_t str_only : 1;
} attr_t;

typedef struct __attribute__((packed)) {
    uint32_t node_num : 24;
    uint8_t entry_len;
    attr_t attrs;
} header_t;

typedef struct __attribute__((packed)) {
    uint32_t node : 24;
    uint8_t level;
} search_node_t;

extern header_t _header;
extern child_t _child;
extern char _buf[128];

void seek(uint32_t addr);
void read_string(void);
void read_header(void);
void read_child(void);

bool stroke_to_string(uint32_t stroke, char *buf, uint8_t *len);
uint32_t qmk_chord_to_stroke(uint8_t chord[6]);
void search_on_nodes(search_node_t *nodes, uint8_t *size, uint32_t stroke, uint32_t *max_level_node, uint8_t *max_level);
