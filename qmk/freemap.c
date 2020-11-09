#include "stroke.h"
#include "flash.h"

static uint32_t get_start(uint8_t lvl) {
    switch (lvl) {
        case 0: return FREEMAP_LVL_0;
        case 1: return FREEMAP_LVL_1;
        case 2: return FREEMAP_LVL_2;
        case 3: return FREEMAP_LVL_3;
    }
    return -1;
}

static uint32_t find_free(const uint32_t word, const uint8_t start, const uint8_t block) {
    const uint8_t size = pow(2, block);
    uint32_t mask = pow(2, size) - 1;
    for (uint8_t i = 0; i < 32; i += size) {
        if (i < start) {
            mask <<= size;
            continue;
        }
        if ((word & mask) == mask) {
            return i;
        }
        mask <<= size;
    }
    return -1;
}

static uint8_t _req(const uint8_t lvl, const uint32_t word, const uint8_t block, uint32_t *ret_ind) {
    // NOTE: `word` is word (32-bit) index, *not byte*
    const uint8_t this_lvl_block = lvl == 0 ? block : 0;
    const uint32_t start_ind = get_start(lvl);
    const uint32_t word_addr = 4 * (start_ind + word);
    uint32_t alloc_word;
    flash_read(word_addr, (uint8_t *) &alloc_word, 4);
    const uint8_t size = pow(2, block);
    const uint32_t mask = pow(2, size) - 1;
    for (uint8_t start = 0; start < 32; start ++) {
        uint32_t ind = find_free(alloc_word, start, this_lvl_block);
        if (ind != -1) {
            if (lvl != 0) {
                uint32_t i;
                uint8_t full = _req(lvl - 1, ind, block, &i);
                if (i == -1) {
                    continue;
                } else {
                    ind = i;
                    if (full) {
                        uint32_t write_word = ~(mask << ind);
                        flash_write(word_addr, (uint8_t *) &write_word, 4);
                        alloc_word &= write_word;
                    }
                }
            } else {
                uint32_t write_word = ~(mask << ind);
                flash_write(word_addr, (uint8_t *) &write_word, 4);
                alloc_word &= write_word;
            }
            *ret_ind = word * 32 + ind;
            return alloc_word == 0;
        }
    }
    *ret_ind = -1;
    return 0;
}

// 0xFF is None, since we don't have that much space
uint32_t freemap_req(uint8_t block) {
    uint32_t ind;
    _req(3, 0, block, &ind);
    return ind;
}
