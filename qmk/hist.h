#pragma once

#include "stroke.h"

#define HIST_SIZE 32
#define HIST_MASK 0x1F
#define HIST_LIMIT(a) ((a) & HIST_MASK)

#ifdef STENO_NOORTHOGRAPHY
#define HIST_GET_ORTHO_LEN(hist) (BUCKET_GET_STROKES_LEN(hist->bucket))
#else
#define HIST_GET_ORTHO_LEN(hist) (hist->ortho_len)
#endif

typedef struct __attribute__((packed)) {
    uint8_t space : 1;
    uint8_t cap : 2;
    uint8_t glue : 1;
} state_t;

typedef struct __attribute__((packed)) {
    uint8_t len;
    state_t state;
#ifndef STENO_NOORTHOGRAPHY
    uint8_t ortho_len;       // Length of orthographic entries' total strokes
#endif
    uint32_t stroke : 24;
    // Pointer + strokes length of the bucket; invalid if 0 or -1
    uint32_t bucket;
#ifndef STENO_NOORTHOGRAPHY
    uint8_t end_buf[8];
#endif
} history_t;

#ifndef QMK_KEYBOARD
void register_code(const uint32_t keycode);
void unregister_code(const uint32_t keycode);
void tap_code(const uint32_t keycode);
void send_char(const char ascii_code);
const char *decode_utf8(const char *str, int32_t *const code_point);
#endif

void hist_undo(uint8_t h_ind);
history_t *hist_get(uint8_t ind);
state_t process_output(uint8_t h_ind);
