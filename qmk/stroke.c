// To deal with operations related with strokes, and the file system
#include "stroke.h"
#include "hist.h"
#include "stdbool.h"

header_t _header;
child_t _child;
char _buf[128];
#ifdef USE_SPI_FLASH
static uint32_t flash_addr = 0;
#else
FATFS fat_fs;
#endif

void seek(uint32_t addr) {
#ifdef USE_SPI_FLASH
    flash_addr = addr;
#else
    uprintf("seek: %X\n", pf_lseek(addr));
#endif
}

void read_string(void) {
#ifdef USE_SPI_FLASH
    flash_read(flash_addr, (uint8_t *) _buf, _header.entry_len);
#else
    UINT br;
    pf_read(_buf, _header.entry_len, &br);
#endif
    _buf[_header.entry_len] = 0;
}

void read_header(void) {
#ifdef USE_SPI_FLASH
    flash_read(flash_addr, (uint8_t *) &_header, sizeof(header_t));
    seek(flash_addr + sizeof(header_t));
#else
    UINT br;
    pf_read(&_header, sizeof(header_t), &br);
#endif
}

void read_child(void) {
#ifdef USE_SPI_FLASH
    flash_read(flash_addr, (uint8_t *) &_child, sizeof(child_t));
    seek(flash_addr + sizeof(child_t));
#else
    UINT br;
    pf_read(&_child, sizeof(child_t), &br);
#endif
}

uint32_t hash_stroke(uint32_t stroke) {
    uint32_t hash = 0x811c9dc5;
    hash *= 0x01000193;
    hash ^= (stroke) & 0xFF;
    hash *= 0x01000193;
    hash ^= (stroke >> 8) & 0xFF;
    hash *= 0x01000193;
    hash ^= (stroke >> 16) & 0xFF;
    hash *= 0x01000193;
    /* hash ^= (stroke >> 24) & 0xFF; */
    return hash;
}

// Find a stroke in the children of a specific node
uint32_t node_find_stroke(uint32_t header_ptr, uint32_t stroke) {
    seek(header_ptr);
    read_header();
    uint8_t max_collisions;
    uint32_t children_ptr = header_ptr + sizeof(header_t) + _header.entry_len;
    if (_header.node_num < MAX_COLLISIONS) {
        seek(children_ptr);
        max_collisions = _header.node_num;
    } else {
        uint32_t child_ptr = children_ptr + (hash_stroke(stroke) % _header.node_num) * sizeof(child_t);
        seek(child_ptr);
        max_collisions = MAX_COLLISIONS;
    }

    for (uint8_t collisions = 0; collisions < max_collisions; collisions ++) {
        read_child();
        if (stroke == _child.stroke) {
            return _child.addr;
        }
        if (_child.stroke == 0xffffff) {
            return 0;
        }
    }
    return 0;
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
        *ret_len = len;
        return true;
    }

    char *KEYS = "#STKPWHRAO*EUFRPBLGTSDZ";
    bool has_mid = false;
    for (int8_t i = 22; i >= 0; i --) {
        if (stroke & ((uint32_t) 1 << i)) {
            if (i < 10) {
                if (!has_mid) {
                    buf[len++] = '-';
                    has_mid = true;
                }
            } else if (i <= 14) {
                has_mid = true;
            }
            buf[len++] = KEYS[22 - i];
        }
    }
    buf[len] = 0;
    *ret_len = len;
    return false;
}

uint32_t qmk_chord_to_stroke(uint8_t chord[6]) {
    uint32_t keys = ((uint32_t) chord[5] & 1)
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

// Searches on multiple nodes, for use with the top level, and tries to find the longest match
void search_on_nodes(search_node_t *nodes, uint8_t *size, uint32_t stroke, uint32_t *max_level_node, uint8_t *max_level) {
    steno_debug("search_on_nodes()\n");
    uint8_t _size = *size;
    *size = 0;
    for (uint8_t i = 0; i <= _size; i ++) {
        bool last = i == _size;
        // Search root node at the end
        uint32_t node = last ? 0 : nodes[i].node;
        uint32_t next_node = node_find_stroke(node, stroke);
#ifdef STENO_DEBUG
        char buf[24];
        uint8_t _len = 0;
        stroke_to_string(stroke, buf, &_len);
        xprintf("  %lX + %s -> %lX\n", node, buf, next_node);
#endif
        if (!next_node) {
            if (!last) {
                i = _size - 1;
            }
            continue;
        }
        seek(next_node);
        read_header();
        uint32_t node_num = _header.node_num;
        uint8_t next_level = last ? 1 : nodes[i].level + 1;
        if (_header.attrs.present && next_level > *max_level) {
            *max_level = next_level;
            *max_level_node = next_node;
        }
        xprintf("    node_num: %lu, next_level: %u\n", node_num, next_level);
        if (node_num) {
            nodes[*size].node = next_node;
            nodes[*size].level = next_level;
            (*size)++;
            if (*size >= SEARCH_NODES_SIZE) {
                xprintf("Search nodes full!\n");
                return;
            }
        } else {
            return;
        }
    }
    steno_debug("  -> max_level: %u, max_level_node: %lX\n", *max_level, *max_level_node);
}
