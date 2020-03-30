#ifndef _STENO_H_
#define _STENO_H_

#include "quantum.h"
#include <stdint.h>

#define S_N1  ((uint32_t) 1 << 27)
#define S_N2  ((uint32_t) 1 << 26)
#define S_SU  ((uint32_t) 1 << 25)
#define S_SD  ((uint32_t) 1 << 24)
#define S_TL  ((uint32_t) 1 << 23)
#define S_K   ((uint32_t) 1 << 22)
#define S_PL  ((uint32_t) 1 << 21)
#define S_W   ((uint32_t) 1 << 20)
#define S_H   ((uint32_t) 1 << 19)
#define S_RL  ((uint32_t) 1 << 18)
#define S_A   ((uint32_t) 1 << 17)
#define S_O   ((uint32_t) 1 << 16)
#define S_ST1 ((uint32_t) 1 << 15)
#define S_ST2 ((uint32_t) 1 << 14)
#define S_ST3 ((uint32_t) 1 << 13)
#define S_ST4 ((uint32_t) 1 << 12)
#define S_E   ((uint32_t) 1 << 11)
#define S_U   ((uint32_t) 1 << 10)
#define S_F   ((uint32_t) 1 << 9)
#define S_RR  ((uint32_t) 1 << 8)
#define S_PR  ((uint32_t) 1 << 7)
#define S_B   ((uint32_t) 1 << 6)
#define S_L   ((uint32_t) 1 << 5)
#define S_G   ((uint32_t) 1 << 4)
#define S_TR  ((uint32_t) 1 << 3)
#define S_SR  ((uint32_t) 1 << 2)
#define S_D   ((uint32_t) 1 << 1)
#define S_Z   ((uint32_t) 1 << 0)

typedef struct {
    uint32_t input;
    uint32_t addr;
} child_t;

typedef struct {
    uint16_t node_num;
    uint8_t level;
    uint8_t str_len;
} header_t;

typedef struct {
    uint32_t input;
    uint32_t node;
    uint8_t replaced;
} history_node_t;

uint32_t node_find_input(uint32_t header_ptr, uint32_t input);
void read_file_at(int32_t addr, void *dest, uint16_t size);
void read_header_at(int32_t addr);
void read_child_at(int32_t addr);
void read_string_at(int32_t addr);

void hist_inc(void);
void hist_replace(uint8_t n, uint32_t node);
void hist_add(uint32_t node);
void hist_add_raw(uint32_t input);
void hist_exec(void);
void hist_undo(void);

#endif
