#pragma once

// Init the underlying storage
void store_init(void);
void store_read(uint32_t const offset, uint8_t *const buf, const uint8_t len);
// Flush the erases and writes in the underlying storage; may be used for optimizations
void store_flush(void);
// Perform a raw/direct write to the underlying storage; this is when we know we are only clearing
// bits
void store_write_direct(const uint32_t offset, const uint8_t *const buf, const uint8_t len);
// Perform a partial erase to the underlying storage, and len is assumed to be smaller than the
// smallest Erase Unit. Thus a erase (maybe of the whole page) is needed and data other than the
// section we want to erase need to be copied to some buffer and copied back
void store_erase_partial(const uint32_t offset, const uint8_t len);
// Starting a complete rewrite to the whole dictionary; corresponding to a device/region erase in
// flash. Assumed that `len` is smaller than flash page size
void store_rewrite_start(void);
// Incremental addition to a total rewrite; buffer is always of the same length
void store_rewrite_write(const uint32_t offset, const uint8_t *const buf);

#define STORE_REWRITE_PACKET_SIZE 64        // FIXME

#include "steno.h"
#ifdef STENO_DEBUG_FLASH
extern uint8_t flash_debug_enable;
#endif
