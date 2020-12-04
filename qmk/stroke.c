// To deal with operations related with strokes, and the file system
#include <string.h>

#include "stroke.h"
#include "hist.h"
#include "stdbool.h"

#ifdef USE_SPI_FLASH
#include "flash.h"
#else
#include "sd/pff.h"
#endif

uint32_t last_entry_ptr;
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
    steno_debug_ln("  find_strokes():");
#endif
    uint32_t hash = FNV_SEED;
    for (uint8_t i = 0; i < len; i ++) {
#ifdef STENO_DEBUG_STROKE
        steno_debug_ln("    [%d] = %02X%02X%02X", i, strokes[3 * i + 2], strokes[3 * i + 1], strokes[3 * i]);
#endif
        hash_stroke_ptr(&hash, strokes + 3 * i);
    }
#ifdef STENO_DEBUG_STROKE
    steno_debug_ln("    hash: %08lX", hash);
#endif
    uint32_t bucket_ind = 4 * (hash & 0xFFFFF);   // Lower 20 bits, mul 4 to byte address
#ifdef STENO_DEBUG_STROKE
    steno_debug_ln("    bucket_ind: %06lX", bucket_ind);
#endif
    for (; ; bucket_ind += 4) {
        flash_read(bucket_ind, (uint8_t *) &last_entry_ptr, 4);
#ifdef STENO_DEBUG_STROKE
        steno_debug_ln("    entry: %06lX", last_entry_ptr);
#endif
        if (free) {
            if (last_entry_ptr == 0xFFFFFF) {
                // FIXME Special usage
                last_entry_ptr = bucket_ind;
                return;
            } else {
                continue;
            }
        }
        const uint8_t entry_stroke_len = ENTRY_GET_STROKES_LEN(last_entry_ptr);
        if (entry_stroke_len == 0 || entry_stroke_len == 0xF) {
            last_entry_ptr = 0;
            return;
        }
        if (entry_stroke_len != len) {
            continue;
        }
        const uint8_t byte_len = 3 * len;
        const uint32_t byte_ptr = ENTRY_GET_ADDR(last_entry_ptr);
        flash_read(byte_ptr, entry_buf, byte_len);
#ifdef STENO_DEBUG_STROKE
        steno_debug_ln("    strokes:");
        for (uint8_t i = 0; i < len; i ++) {
            steno_debug_ln("      [%d] = %02X%02X%02X", i, entry_buf[3 * i + 2], entry_buf[3 * i + 1], entry_buf[3 * i]);
        }
#endif
        if (memcmp(strokes, entry_buf, byte_len) == 0) {
            const uint8_t entry_len = ENTRY_GET_ENTRY_LEN(last_entry_ptr);
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
    uint32_t entry_ptr = 0;
    uint8_t strokes[3 * max_strokes_len];
    for (uint8_t i = 0; i < max_strokes_len; i ++) {
        history_t *old_hist = hist_get(HIST_LIMIT(h_ind - i));
        const uint8_t strokes_len = ENTRY_GET_STROKES_LEN(old_hist->entry);
#ifdef STENO_DEBUG_STROKE
        steno_debug_ln("  hist[%d].strokes_len = %d", HIST_LIMIT(h_ind - i), strokes_len);
#endif
        if (i > 0 && strokes_len > 1) {
            i += strokes_len - 1;
            continue;
        }
        const uint32_t stroke = old_hist->stroke;
#ifdef STENO_DEBUG_STROKE
        steno_debug_ln("  [%d] = %06lX", i, stroke);
#endif
        uint8_t *strokes_start = &strokes[3 * (max_strokes_len - 1 - i)];
        memcpy(strokes_start, &stroke, 3);
        find_strokes(strokes_start, i + 1, 0);
        if (last_entry_ptr != 0) {
            entry_ptr = last_entry_ptr;
#ifdef STENO_DEBUG_STROKE
            steno_debug_ln("  entry: %06lX", entry_ptr);
#endif
        }
    }
    last_entry_ptr = entry_ptr;
}

void read_entry(uint32_t entry_ptr) {
    const uint32_t byte_ptr = ENTRY_GET_ADDR(entry_ptr);
    const uint8_t entry_len = ENTRY_GET_ENTRY_LEN(entry_ptr);
    const uint8_t strokes_len = ENTRY_GET_STROKES_LEN(entry_ptr);
    flash_read(byte_ptr, entry_buf, strokes_len * 3 + 1 + entry_len);
}

void print_strokes(const uint8_t *strokes, const uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        const uint32_t stroke = (uint32_t) strokes[3 * i + 2] << 16 | (uint32_t) strokes[3 * i + 1] << 8 |
                                (uint32_t) strokes[3 * i];
        char buf[24];
        stroke_to_string(stroke, buf, NULL);
        steno_debug("%s/", buf);
    }
    steno_debug_ln("");
}
