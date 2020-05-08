#pragma once

#define HIST_SIZE 30

typedef struct __attribute__((packed)) {
    uint32_t stroke;
    uint32_t node;
    uint8_t replaced;
} history_node_t;

void hist_inc(void);
void hist_replace(uint8_t n, uint32_t node);
void hist_add(uint32_t node);
void hist_add_raw(uint32_t input);
void hist_exec(void);
void hist_undo(void);
