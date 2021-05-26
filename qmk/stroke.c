// To deal with operations related with strokes, and the file system
#include <string.h>

#include "stroke.h"
#include "hist.h"
#include "stdbool.h"

#include "store.h"

uint8_t kvpair_buf[128];

void hash_stroke_ptr(uint32_t *const hash, const uint8_t *const stroke) {
    *hash *= FNV_FACTOR;
    *hash ^= stroke[0];
    *hash *= FNV_FACTOR;
    *hash ^= stroke[1];
    *hash *= FNV_FACTOR;
    *hash ^= stroke[2];
    *hash *= FNV_FACTOR;
}

// Returns if the stroke contains only digits
bool stroke_to_string(const uint32_t stroke, char *const buf, const uint8_t *const ret_len) {
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

uint32_t find_strokes(const uint8_t *const strokes, const uint8_t len, const uint8_t free) {
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
    steno_debug_ln("\n    hash: %08lX, bucket_ind: %06lX", hash, bucket_ind);
#endif
    uint32_t bucket;
    for (; ; bucket_ind += BUCKET_SIZE) {
        store_read(bucket_ind, (uint8_t *) &bucket, BUCKET_SIZE);
#ifdef STENO_DEBUG_STROKE
        steno_debug_ln("    bucket: %08lX", bucket);
#endif
        if (free) {
            if (bucket == 0xFFFFFFFF) {
                return bucket_ind;
            } else {
                continue;
            }
        }
        const uint8_t entry_stroke_len = BUCKET_GET_STROKES_LEN(bucket);
        if (entry_stroke_len == 0 || entry_stroke_len == 0xF) {
            return 0;
        }
        if (entry_stroke_len != len) {
            continue;
        }
        const uint8_t byte_len = STROKE_SIZE * len;
        const uint32_t byte_ptr = BUCKET_GET_ADDR(bucket);
        store_read(byte_ptr, kvpair_buf, byte_len);
#ifdef STENO_DEBUG_STROKE
        steno_debug("      strokes: ");
        for (uint8_t i = 0; i < len; i ++) {
            steno_debug("%02X%02X%02X, ", kvpair_buf[STROKE_SIZE * i + 2], kvpair_buf[STROKE_SIZE * i + 1], kvpair_buf[STROKE_SIZE * i]);
        }
        steno_debug_ln("");
#endif
        if (memcmp(strokes, kvpair_buf, byte_len) == 0) {
            const uint8_t entry_len = BUCKET_GET_ENTRY_LEN(bucket);
            store_read(byte_ptr + byte_len, kvpair_buf + byte_len, entry_len + 1);
            return bucket;
        } else {
            continue;
        }
    }
}

// Searches the dictionary for the appropriate entry to output, and places result in the corresponding history entry.
uint32_t search_entry(const uint8_t h_ind) {
#ifdef STENO_DEBUG_STROKE
    steno_debug_ln("search_entry(%d):", h_ind);
#endif
    uint8_t max_strokes_len = 0;
    if (stroke_start_ind == 0xFF) {
        max_strokes_len = MAX_STROKE_NUM;
    } else if (stroke_start_ind <= h_ind) {
        max_strokes_len = h_ind - stroke_start_ind + 1;
    } else {
        max_strokes_len = h_ind + HIST_SIZE - stroke_start_ind + 1;
    }
    uint32_t max_bucket = 0;
    uint8_t strokes[STROKE_SIZE * max_strokes_len];
    for (uint8_t i = 0; i < max_strokes_len; i ++) {
        history_t const *old_hist = hist_get(HIST_LIMIT(h_ind - i));
        const uint8_t strokes_len = BUCKET_GET_STROKES_LEN(old_hist->bucket);
#ifdef STENO_DEBUG_STROKE
        steno_debug_ln("  hist[%d].strokes_len = %d", HIST_LIMIT(h_ind - i), strokes_len);
#endif
        const uint32_t stroke = old_hist->stroke;
#ifdef STENO_DEBUG_STROKE
        steno_debug_ln("  [%d] = %06lX", i, stroke);
#endif
        if (stroke == 0) {
            break;
        }
        uint8_t *strokes_start = &strokes[STROKE_SIZE * (max_strokes_len - 1 - i)];
        memcpy(strokes_start, &stroke, STROKE_SIZE);
        if (i > 0 && strokes_len > 1) {
            i += strokes_len - 2;
            continue;
        }
        const uint32_t bucket = find_strokes(strokes_start, i + 1, 0);
        if (bucket != 0) {
            max_bucket = bucket;
#ifdef STENO_DEBUG_STROKE
            steno_debug_ln("  bucket: %08lX", bucket);
#endif
        }
    }
    return max_bucket;
}

void read_entry(const uint32_t bucket, uint8_t *const buf) {
    const uint32_t byte_ptr = BUCKET_GET_ADDR(bucket);
    const uint8_t entry_len = BUCKET_GET_ENTRY_LEN(bucket);
    const uint8_t strokes_len = BUCKET_GET_STROKES_LEN(bucket);
    store_read(byte_ptr, buf, strokes_len * STROKE_SIZE + 1 + entry_len);
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
