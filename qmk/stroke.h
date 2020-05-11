#pragma once

#include <stdbool.h>

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

typedef enum {
    ATTR_CAPS_DEFAULT = 0,
    ATTR_CAPS_FORCE_LOWER = 1,
    ATTR_CAPS_KEEP = 2,
    ATTR_CAPS_FORCE_UPPER = 3,
} attr_caps_t;

typedef struct __attribute__((packed)) {
    attr_caps_t caps : 2;
    uint8_t space_prev : 1;
    uint8_t space_after : 1;
    uint8_t glue : 1;
    uint8_t force_all_caps : 1;
    uint8_t force_all_lower : 1;
} attr_t;

typedef struct __attribute__((packed)) {
    uint32_t node_num : 24;
    uint8_t str_len;
    attr_t attrs;
} header_t;

typedef struct __attribute__((packed)) {
    uint32_t node : 24;
    uint8_t level;
} search_node_t;

struct fat_file_struct *file;
header_t _header;
child_t _child;
char _buf[128];

void seek(int32_t addr);
void read_string(void);
void read_header(void);
void read_child(void);

bool stroke_to_string(uint32_t stroke, char *buf, uint8_t *len);
uint32_t qmk_chord_to_stroke(uint8_t chord[6]);
void search_on_nodes(search_node_t *nodes, uint8_t *size, uint32_t stroke, uint32_t *max_level_node, uint8_t *max_level);
