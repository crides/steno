#pragma once

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

typedef struct __attribute__((packed)) {
    uint32_t node_num : 24;
    uint8_t str_len;
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

void stroke_to_string(uint32_t stroke, char *buf);
uint32_t qmk_chord_to_stroke(uint8_t chord[6]);
void search_on_nodes(search_node_t *nodes, uint8_t *size, uint32_t stroke, uint32_t *max_level_node, uint8_t *max_level, uint8_t *max_level_ended);
