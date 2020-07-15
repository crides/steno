#pragma once

#include "stroke.h"

#define HIST_SIZE 32

typedef enum {
    RAW_STROKE,
    NODE_STRING,
} output_type_t;

typedef struct __attribute__((packed)) {
    output_type_t type;
    union {
        uint32_t stroke : 24;
        uint32_t node : 24;
    };
} output_t;

typedef struct __attribute__((packed)) {
    uint8_t space : 1;
    uint8_t cap : 2;
    uint8_t prev_glue : 1;
} state_t;

typedef struct __attribute__((packed)) {
    uint8_t len;
    state_t state;
    uint8_t repl_len : 4;
    uint8_t search_nodes_len : 4;
    search_node_t *search_nodes;
    output_t output;
} history_t;

extern search_node_t search_nodes[SEARCH_NODES_SIZE];
extern uint8_t search_nodes_len;
extern state_t state;

extern history_t history[HIST_SIZE];
extern uint8_t hist_ind;

void hist_add(history_t hist);
void hist_undo(void);
uint8_t process_output(state_t *state, output_t output, uint8_t repl_len);
