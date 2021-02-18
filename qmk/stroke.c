// To deal with operations related with strokes, and the file system
#include <string.h>

#include "stroke.h"
#include "hist.h"
#include "stdbool.h"

#include "flash.h"

uint32_t last_bucket;
uint8_t entry_buf[128];

void hash_stroke_ptr(uint32_t *hash, const uint8_t *stroke) {
    *hash *= FNV_FACTOR;
    *hash ^= stroke[0];
    *hash *= FNV_FACTOR;
    *hash ^= stroke[1];
    *hash *= FNV_FACTOR;
    *hash ^= stroke[2];
    *hash *= FNV_FACTOR;
}

// Returns if the stroke contains only digits
bool stroke_to_string(uint32_t stroke, char *buf, uint8_t *ret_len) {
    uint8_t len = 0;
    const uint32_t DIGIT_BITS = 0x7562A8;
    if (stroke & ((uint32_t) 1 << 22) && !(stroke & ~DIGIT_BITS)) {
        if (stroke & ((uint32_t) 1 << 21)) buf[len++] = '1';
        if (stroke & ((uint32_t) 1 << 20)) buf[len++] = '2';
        if (stroke & ((uint32_t) 1 << 18)) buf[len++] = '3';
        if (stroke & ((uint32_t) 1 << 16)) buf[len++] = '4';
        if (stroke & ((uint32_t) 1 << 14)) buf[len++] = '5';
        if (stroke & ((uint32_t) 1 << 13)) buf[len++] = '0';
        if (stroke & ((uint32_t) 1 << 9)) buf[len++] = '6';
        if (stroke & ((uint32_t) 1 << 7)) buf[len++] = '7';
        if (stroke & ((uint32_t) 1 << 5)) buf[len++] = '8';
        if (stroke & ((uint32_t) 1 << 3)) buf[len++] = '9';
        buf[len] = 0;
        if (ret_len) {
            *ret_len = len;
        }
        return true;
    }

    const char *KEYS = "#STKPWHRAO*EUFRPBLGTSDZ";
    bool has_mid = false;
    for (int8_t i = 22; i >= 0; i --) {
        if (stroke & ((uint32_t) 1 << i)) {
            if (i < 10) {
                if (!has_mid) {
                    buf[len] = '-';
                    len ++;
                    has_mid = true;
                }
            } else if (i <= 14) {
                has_mid = true;
            }
            buf[len] = KEYS[22 - i];
            len ++;
        }
    }
    buf[len] = 0;
    if (ret_len) {
        *ret_len = len;
    }
    return false;
}

uint32_t qmk_chord_to_stroke(const uint8_t chord[6]) {
    const uint32_t keys = ((uint32_t) chord[5] & 1)
        | ((uint32_t) chord[4] & 0x7F) << 1
        | ((uint32_t) chord[3] & 0x3F) << 8
        | ((uint32_t) chord[2] & 0x7F) << (14 - 2)
        | ((uint32_t) chord[1] & 0x7F) << 19
        | ((uint32_t) chord[0] & 0x30) << (26 - 4);

    uint32_t stroke = keys & 0xFFF;
    if (keys & 0xF000) {    // Asterisk
        stroke |= 0x1000;
    }
    stroke |= (keys >> 3) & 0x1FE000; // Left side of asterisk
    if (keys & 0x3000000) { // S-
        stroke |= 0x200000;
    }
    if (keys & 0xC000000) { // #
        stroke |= 0x400000;
    }
    return stroke;
}

void find_strokes(const uint8_t *strokes, const uint8_t len, const uint8_t free) {
#ifdef STENO_DEBUG_STROKE
    steno_debug("  find_strokes(%u):\n    ", free);
#endif
    uint32_t hash = FNV_SEED;
    for (uint8_t i = 0; i < len; i ++) {
#ifdef STENO_DEBUG_STROKE
        steno_debug("%02X%02X%02X, ", strokes[STROKE_SIZE * i + 2], strokes[STROKE_SIZE * i + 1], strokes[STROKE_SIZE * i]);
#endif
        hash_stroke_ptr(&hash, strokes + STROKE_SIZE * i);
    }
    uint32_t bucket_ind = BUCKET_SIZE * (hash & 0xFFFFF);   // Lower 20 bits, mul 4 to byte address
#ifdef STENO_DEBUG_STROKE
    steno_debug_ln("");
    steno_debug_ln("    hash: %08lX, bucket_ind: %06lX", hash, bucket_ind);
#endif
    for (; ; bucket_ind += BUCKET_SIZE) {
        flash_read(bucket_ind, (uint8_t *) &last_bucket, BUCKET_SIZE);
#ifdef STENO_DEBUG_STROKE
        steno_debug_ln("    bucket: %08lX", last_bucket);
#endif
        if (free) {
            if (last_bucket == 0xFFFFFFFF) {
                // FIXME Special usage
                last_bucket = bucket_ind;
                return;
            } else {
                continue;
            }
        }
        const uint8_t entry_stroke_len = BUCKET_GET_STROKES_LEN(last_bucket);
        if (entry_stroke_len == 0 || entry_stroke_len == 0xF) {
            last_bucket = 0;
            return;
        }
        if (entry_stroke_len != len) {
            continue;
        }
        const uint8_t byte_len = STROKE_SIZE * len;
        const uint32_t byte_ptr = BUCKET_GET_ADDR(last_bucket);
        flash_read(byte_ptr, entry_buf, byte_len);
#ifdef STENO_DEBUG_STROKE
        steno_debug("      strokes: ");
        for (uint8_t i = 0; i < len; i ++) {
            steno_debug("%02X%02X%02X, ", entry_buf[STROKE_SIZE * i + 2], entry_buf[STROKE_SIZE * i + 1], entry_buf[STROKE_SIZE * i]);
        }
        steno_debug_ln("");
#endif
        if (memcmp(strokes, entry_buf, byte_len) == 0) {
            const uint8_t entry_len = BUCKET_GET_ENTRY_LEN(last_bucket);
            flash_read(byte_ptr + byte_len, entry_buf + byte_len, entry_len + 1);
            return;
        } else {
            continue;
        }
    }
}

// Searches the dictionary for the appropriate entry to output, and places result in the corresponding history entry.
void search_entry(const uint8_t h_ind) {
#ifdef STENO_DEBUG_STROKE
    steno_debug_ln("search_entry(%d):", h_ind);
#endif
    uint8_t max_strokes_len = 0;
    if (stroke_start_ind == 0xFF) {
        max_strokes_len = 14;    // Max stroke length in an entry
    } else if (stroke_start_ind <= h_ind) {
        max_strokes_len = h_ind - stroke_start_ind + 1;
    } else {
        max_strokes_len = h_ind + HIST_SIZE - stroke_start_ind + 1;
    }
    uint32_t bucket = 0;
    uint8_t strokes[STROKE_SIZE * max_strokes_len];
    for (uint8_t i = 0; i < max_strokes_len; i ++) {
        history_t *old_hist = hist_get(HIST_LIMIT(h_ind - i));
        const uint8_t strokes_len = BUCKET_GET_STROKES_LEN(old_hist->bucket);
#ifdef STENO_DEBUG_STROKE
        steno_debug_ln("  hist[%d].strokes_len = %d", HIST_LIMIT(h_ind - i), strokes_len);
#endif
        if (i > 0 && strokes_len > 1) {
            i += strokes_len - 1;
            continue;
        }
        const uint32_t stroke = old_hist->stroke;
        if (stroke == 0) {
            break;
        }
#ifdef STENO_DEBUG_STROKE
        steno_debug_ln("  [%d] = %06lX", i, stroke);
#endif
        uint8_t *strokes_start = &strokes[STROKE_SIZE * (max_strokes_len - 1 - i)];
        memcpy(strokes_start, &stroke, STROKE_SIZE);
        find_strokes(strokes_start, i + 1, 0);
        if (last_bucket != 0) {
            bucket = last_bucket;
#ifdef STENO_DEBUG_STROKE
            steno_debug_ln("  bucket: %06lX", bucket);
#endif
        }
    }
    last_bucket = bucket;
}

void read_entry(uint32_t bucket) {
    const uint32_t byte_ptr = BUCKET_GET_ADDR(bucket);
    const uint8_t entry_len = BUCKET_GET_ENTRY_LEN(bucket);
    const uint8_t strokes_len = BUCKET_GET_STROKES_LEN(bucket);
    flash_read(byte_ptr, entry_buf, strokes_len * STROKE_SIZE + 1 + entry_len);
}

void print_strokes(const uint8_t *strokes, const uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        const uint32_t stroke = STROKE_FROM_PTR(&strokes[STROKE_SIZE * i]);
        char buf[24];
        stroke_to_string(stroke, buf, NULL);
        steno_debug("%s/", buf);
    }
    steno_debug_ln("");
}
