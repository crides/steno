/* #include "stroke.h" */
/* #include "store.h" */
/* #include "steno.h" */
#include <stdint.h>
#include <string.h>

#define KVPAIR_BLOCK_START  0x400000
#define FREEMAP_START       0xF00000

#define FREEMAP_LVL_0 FREEMAP_START
#define FREEMAP_LVL_1 ((1ul << 20) / 32 * 4 + FREEMAP_LVL_0)
#define FREEMAP_LVL_2 ((1ul << 20) / 32 / 32 * 4 + FREEMAP_LVL_1)
#define FREEMAP_LVL_3 ((1ul << 20) / 32 / 32 / 32 * 4 + FREEMAP_LVL_2)

#define ALLOCATOR_SIZE (sizeof(uint32_t) * (1 + 32 + 32*32 + 32*32*32))

static uint8_t mem[ALLOCATOR_SIZE] = {0};

void store_read(uint32_t const offset, uint8_t *const buf, const uint8_t len) {
    const uint32_t fm_off = offset - FREEMAP_START;
    if (fm_off + len > ALLOCATOR_SIZE) {
        goto ERROR;
    }
    for (uint32_t i = 0; i < len; i ++) {
        buf[i] = mem[fm_off + i];
    }
    /* memcpy(buf, &mem[fm_off], len); */
    return;
ERROR:
    return;
}

void store_write_direct(const uint32_t offset, const uint8_t *const buf, const uint8_t len) {
    const uint32_t fm_off = offset - FREEMAP_START;
    if (fm_off + len > ALLOCATOR_SIZE) {
        goto ERROR;
    }
    for (uint32_t i = 0; i < len; i ++) {
        mem[fm_off + i] &= buf[i];
    }
    return;
ERROR:
    return;
}

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
    const uint32_t shifted = 1 << size;
    uint32_t mask = shifted - 1;
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

extern int __VERIFIER_nondet_int();

int main() {
    /* for (uint32_t i = 0; i < 10; i ++) { */
        const uint8_t blok = __VERIFIER_nondet_int() % 5;   // [0, 4]
        const uint32_t addr = freemap_req(blok);
        if (addr == -1) {
            goto ERROR;
        }
        const uint32_t size = (1 << blok) * 16;
        if (addr % size != 0) {
            goto ERROR;
        }
    /* } */
    return 0;
ERROR:
    return 1;
}
