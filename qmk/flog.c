// Utilities to provide flash-based logging
#include "eeprom.h"
#include "store.h"
#include "stroke.h"
#include "steno.h"
#include "sendchar.h"

#define LOG_ADDR_ADDR ((uint32_t *) 128)
static uint32_t log_addr = FLOG_START;
static uint32_t erased_till = FLOG_START + 0x1000;
static uint8_t buf[128] = {'!'};

// HACK uses private/platform specific interface
void flash_erase_4k(const uint32_t addr);

static int8_t flog_handle_char(uint8_t c) {
    static uint8_t log_buf_size = 1;
    sendchar(c);
    buf[log_buf_size++] = c;
    if (c == '\n') {
        if (log_addr + log_buf_size >= STORE_END) {
            log_addr = FLOG_START;
            erased_till = log_addr;
        }
        const uint32_t cur_page_end = (log_addr & 0xFFFFFF00) + 0x100;
        if (log_addr + log_buf_size > cur_page_end) {
            log_addr = cur_page_end;
        }
        if (log_addr + log_buf_size > erased_till) {
            flash_erase_4k(erased_till);
            erased_till += 0x1000;
        }
        store_flush();
        store_write_direct(log_addr, buf, log_buf_size);
        log_addr += log_buf_size;
        log_buf_size = 1;
        store_flush();
    }
    return 0;
}

void flog_init(void) {
    // Assuming storage is inited
    const uint32_t stored = eeprom_read_dword(LOG_ADDR_ADDR);
    if (stored >= FLOG_START && stored < STORE_END) {
        log_addr = stored;
        erased_till = (log_addr & 0xFFF000) + 0x1000;
    }
    print_set_sendchar(flog_handle_char);
}

void flog_finish_cycle(void) {
    steno_debug_ln("log_addr: %06lX, erased_till: %06lX", log_addr, erased_till);
    eeprom_update_dword(LOG_ADDR_ADDR, log_addr);
}
