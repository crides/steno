// To deal with operations related with strokes, and the file system
#include "quantum.h"
#include "stroke.h"
#include "stdbool.h"
#include "sdcard/fat.h"

struct fat_file_struct *file;
header_t _header;
child_t _child;
char _buf[128];

void seek(int32_t addr) {
    fat_seek_file(file, &addr, FAT_SEEK_SET);
}

void read_string(void) {
    fat_read_file(file, (uint8_t *) _buf, _header.str_len);
    _buf[_header.str_len] = 0;
}

void read_header(void) {
    fat_read_file(file, (uint8_t *) &_header, sizeof(header_t));
}

void read_child(void) {
    fat_read_file(file, (uint8_t *) &_child, sizeof(child_t));
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

uint32_t node_find_stroke(uint32_t header_ptr, uint32_t stroke) {
    seek(header_ptr);
    read_header();
    uint32_t children_ptr = header_ptr + sizeof(header_t) + _header.str_len;
    if (_header.node_num < MAX_COLLISIONS) {
        seek(children_ptr);
    } else {
        uint32_t child_ptr = children_ptr + (hash_stroke(stroke) % _header.node_num) * sizeof(child_t);
        seek(child_ptr);
    }

    uint8_t collisions = 0;
    while (collisions < MAX_COLLISIONS) {
        read_child();
        if (stroke == _child.stroke) {
            return _child.addr;
        }
        if (!_child.stroke) {
            return 0;
        }
        collisions ++;
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

void search_on_nodes(search_node_t *nodes, uint8_t *size, uint32_t stroke, uint32_t *max_level_node, uint8_t *max_level) {
    uint8_t _size = *size;
    *size = 0;
    for (uint8_t i = 0; i <= _size; i ++) {
        // Search root node at the end
        uint32_t node = i == _size ? 0 : nodes[i].node;
        uint32_t next_node = node_find_stroke(node, stroke);
        if (!next_node) {
            continue;
        }
        seek(next_node);
        read_header();
        uint8_t next_level = i == _size ? 1 : nodes[i].level + 1;
        uint32_t node_num = _header.node_num;
        if (_header.str_len || _header.attrs.present) {
            if (next_level > *max_level) {
                *max_level = next_level;
                *max_level_node = next_node;
                read_string();
            }
        }
        if (node_num) {
            nodes[*size].node = next_node;
            nodes[*size].level = next_level;
            (*size)++;
            if (*size >= SEARCH_NODES_SIZE) {
                xprintf("Search nodes full!\n");
                return;
            }
        } else {
            if (next_node == *max_level_node) {    // Max level ended
                *size = 0;
                xprintf("max_level ends\n");
                return;         // FIXME Should we return early here?
            }
        }
    }
}
