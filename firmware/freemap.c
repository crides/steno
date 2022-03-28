#include "stroke.h"
#include "store.h"
#include "steno.h"

static uint32_t get_offset(uint8_t lvl) {
    switch (lvl) {
        case 0: return FREEMAP_LVL_0;
        case 1: return FREEMAP_LVL_1;
        case 2: return FREEMAP_LVL_2;
        case 3: return FREEMAP_LVL_3;
    }
    return -1;
}

static uint8_t _req(const uint8_t lvl, const uint32_t word, const uint8_t block, uint32_t *ret_ind) {
    // NOTE: `word` is word (32-bit) index, *not byte*
    const uint8_t this_lvl_block = lvl == 0 ? block : 0;
    const uint32_t offset = get_offset(lvl);
    const uint8_t size = 1 << this_lvl_block;
    uint32_t mask = (1 << size) - 1;
    const uint32_t word_addr = offset + 4 * word;
    uint32_t alloc_word;
    store_read(word_addr, (uint8_t *) &alloc_word, 4);
#ifdef STENO_DEBUG_FLASH
    steno_debug_ln("lvl %u bloq %u word %lu alok " DWF("08"), lvl, block, word, alloc_word);
#endif
    for (uint8_t i = 0; i < 32; i += size, mask <<= size) {
        if ((alloc_word & mask) == mask) {
            const uint32_t sub_ind = i + word * 32;
            const uint32_t write_word = ~mask;
#ifdef STENO_DEBUG_FLASH
            steno_debug_ln("sind %u", i);
#endif
            if (lvl == 0) {
                store_write_direct(word_addr, (uint8_t *) &write_word, 4);
                alloc_word &= write_word;
                *ret_ind = sub_ind;
            } else {
                uint32_t ret;
                const uint8_t full = _req(lvl - 1, sub_ind, block, &ret);
                if (ret == -1) {
                    continue;
                } else {
                    if (full) {
                        store_write_direct(word_addr, (uint8_t *) &write_word, 4);
                        alloc_word &= write_word;
                    }
                    *ret_ind = ret;
                }
            }
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
    if (ind < (FREEMAP_START - KVPAIR_BLOCK_START)) {
        return ind;
    } else {
        return -1;
    }
}
