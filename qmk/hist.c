// History management and output control
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>

#include "hist.h"
#include "steno.h"
#include "flash.h"
#include "process_keycode/process_unicode_common.h"
#include "dict_editing.h"
#include "lcd.h"

static history_t history[HIST_SIZE];

extern char last_trans[128];
extern uint8_t last_trans_size;

static void dict_edit_puts(char *str) {
    while (*str && entry_buf_len < 255) {
        page_buffer[entry_buf_len ++] = *str;
        lcd_putc(*str, 2);
        str ++;
    }
}

static void steno_back(void) {
    if (editing_state != ED_IDLE) {
        if (entry_buf_len > 0) {
            entry_buf_len --;
            select_lcd();
            lcd_back(2);
            unselect_lcd();
        }
    } else {
        tap_code(KC_BSPC);
    }
}

static void steno_send_char(char c) {
    if (editing_state != ED_IDLE) {
        if (entry_buf_len < 255) {
            page_buffer[entry_buf_len ++] = c;
            lcd_putc(c, 2);
        }
    } else {
        send_char(c);
    }
#ifdef STENO_DEBUG_HIST
    steno_debug("%c", c);
#endif
}

static uint8_t steno_send_unicode(uint32_t u) {
#ifdef STENO_DEBUG_HIST
    if (u < 0xFFFF) {
        steno_debug("\\u%04lX", u);
    } else {
        steno_debug("\\U%06lX", u);
    }
#endif
    if (editing_state != ED_IDLE) {
        char buf[16];
        uint8_t len;
        if (u < 0xFFFF) {
            snprintf(buf, 16, "\\u%04lX", u);
            len = 6;
        } else {
            snprintf(buf, 16, "\\U%06lX", u);
            len = 8;
        }
        dict_edit_puts(buf);
        return len;
    } else {
        register_unicode(u);
        return 1;
    }
}

static uint8_t steno_send_keycodes(uint8_t *keycodes, uint8_t len) {
#ifdef STENO_DEBUG_HIST
    steno_debug("keys(%u):", len);
#endif
    char buf[16];
    uint8_t output_len = 3;
    uint8_t mods = 0;
    if (editing_state != ED_IDLE) {
        dict_edit_puts("\\k[");
    }
    for (uint8_t i = 0; i < len; i++) {
        if ((keycodes[i] & 0xFC) == 0xE0) {
            uint8_t mod = keycodes[i] & 0x03;
            uint8_t mod_char;
            switch (mod) {
            case 0: mod_char = 'c'; break;
            case 1: mod_char = 's'; break;
            case 2: mod_char = 'a'; break;
            case 3: mod_char = 'g'; break;
            }
            uint8_t mod_mask = 1 << mod;
            if (mods & mod_mask) {
                if (editing_state != ED_IDLE) {
                    dict_edit_puts(")");
                    output_len += 1;
                } else {
                    unregister_code(keycodes[i]);
                }
#ifdef STENO_DEBUG_HIST
                steno_debug(" %c", mod_char);
#endif
            } else {
                if (editing_state != ED_IDLE) {
                    snprintf(buf, 16, "\\%c(", mod_char);
                    dict_edit_puts(buf);
                    output_len += 3;
                } else {
                    register_code(keycodes[i]);
                }
#ifdef STENO_DEBUG_HIST
                steno_debug(" %c", toupper(mod_char));
#endif
            }
            mods ^= mod_mask;
        } else {
#ifdef STENO_DEBUG_HIST
            steno_debug(" %02X", keycodes[i]);
#endif
            if (editing_state != ED_IDLE) {
                snprintf(buf, 16, " %02X", keycodes[i]);
                dict_edit_puts(buf);
                output_len += 3;
            } else {
                tap_code(keycodes[i]);
            }
        }
    }
#ifdef STENO_DEBUG_HIST
    steno_debug("\n");
#endif
    if (editing_state != ED_IDLE) {
        dict_edit_puts("]");
        return output_len + 1;
    } else {
        return 0;
    }
}

inline history_t *hist_get(uint8_t ind) {
    return &history[ind];
}

// Undo the last history entry. First delete the output, and then start from the initial state of the
// multi-stage input, and rebuild the output from there.
void hist_undo(uint8_t h_ind) {
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("hist_undo()");
#endif
    history_t *hist = hist_get(h_ind);
    uint8_t len = hist->len;
    if (!len) {
        steno_error_ln("Invalid current history entry");
        steno_back();
        return;
    }

#ifdef STENO_DEBUG_HIST
    steno_debug_ln("  bspc len: %u", len);
#endif
    for (uint8_t i = 0; i < len; i++) {
        steno_back();
    }
    uint8_t strokes_len = ENTRY_GET_LEN(hist->entry);
    uint8_t repl_len = strokes_len > 1 ? strokes_len - 1 : 0;
    for (uint8_t i = 0; i < repl_len; i++) {
        uint8_t old_hist_ind = HIST_LIMIT(h_ind + i - repl_len);
        history_t *old_hist = hist_get(old_hist_ind);
#ifdef STENO_DEBUG_HIST
        steno_debug_ln("  old_hist_ind: %u", old_hist_ind);
#endif
        if (!old_hist->len) {
            hist->len = 0;
            steno_error_ln("Invalid prev hist entry");
            return;
        }
        process_output(old_hist_ind);
    }
    return;
}

// Process the output. If it's a raw stroke (no nodes found for the input), then just output the stroke;
// otherwise, load the entry, perform the necessary transformations for capitalization, and output according
// to the bytes. Also takes care of outputting key codes and Unicode characters.
state_t process_output(uint8_t h_ind) {
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("process_output()");
#endif
    // TODO optimization: compare beginning of current and string to replace
    history_t *hist = hist_get(h_ind);
    uint8_t strokes_len = ENTRY_GET_LEN(hist->entry);
    {
        uint8_t repl_len = strokes_len > 1 ? strokes_len - 1 : 0;
        int8_t counter = repl_len;
        while (counter > 0) {
            uint8_t old_hist_ind = HIST_LIMIT(h_ind + counter - repl_len - 1);
            history_t *old_hist = hist_get(old_hist_ind);
#ifdef STENO_DEBUG_HIST
            steno_debug_ln("  old_hist_ind: %u, bspc len: %u", old_hist_ind, old_hist->len);
#endif
            if (!old_hist->len) {
                hist->len = 0;
                steno_error_ln("Invalid prev hist entry");
                break;
            }
            for (uint8_t j = 0; j < old_hist->len; j++) {
                steno_back();
            }
            uint8_t old_strokes_len = ENTRY_GET_LEN(old_hist->entry);
            uint8_t old_repl_len = old_strokes_len > 1 ? old_strokes_len - 1 : 0;
            counter -= old_repl_len + 1;
        }
    }

    state_t old_state = hist->state, new_state = old_state;
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("  old_state: space: %u, cap: %u, glue: %u", old_state.space, old_state.cap, old_state.glue);
#endif

    if (hist->entry == 0) {
#ifdef STENO_DEBUG_HIST
        steno_debug_ln("  stroke: %lX", hist->stroke & 0xFFFFFF);
#endif
        char buf[24];
        if (stroke_to_string(hist->stroke, buf, &hist->len)) {
            new_state.glue = 1;
        }
#ifdef STENO_DEBUG_HIST
        steno_debug("  output: '");
#endif
        if (old_state.space && !(old_state.glue && new_state.glue)) {
            steno_send_char(' ');
            hist->len++;
        }
        send_string(buf);
#ifdef STENO_DEBUG_HIST
        steno_debug_ln("%s'", buf);
        steno_debug_ln("  -> %u", hist->len);
#endif
        return new_state;
    }

    uint8_t entry_len;
    uint32_t entry_ptr = hist->entry;
    {
        uint32_t byte_ptr = (entry_ptr & 0xFFFFF0) + 0x300000;
        uint8_t stroke_byte_len = 3 * strokes_len;
        flash_read(byte_ptr, entry_buf, stroke_byte_len + 1);
        entry_len = entry_buf[stroke_byte_len];
        flash_read(byte_ptr + stroke_byte_len + 1, entry_buf + stroke_byte_len + 1, entry_len + 1);
    }
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("  entry_len: %u", entry_len);
#endif

    attr_t attr = *((attr_t *) &entry_buf[(entry_ptr & 0x0F) * 3 + 1]);
    new_state.space = attr.space_after;
    new_state.glue = attr.glue;
    uint8_t space = old_state.space && attr.space_prev && entry_len && !(old_state.glue && attr.glue);
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("  entry_buf:");
    steno_debug("    %02X%02X%02X%02X", entry_buf[0], entry_buf[1], entry_buf[2], entry_buf[3]);
    steno_debug(" %02X%02X%02X%02X", entry_buf[4], entry_buf[5], entry_buf[6], entry_buf[7]);
    steno_debug(" %02X%02X%02X%02X", entry_buf[8], entry_buf[9], entry_buf[10], entry_buf[11]);
    steno_debug_ln(" %02X%02X%02X%02X", entry_buf[12], entry_buf[13], entry_buf[14], entry_buf[15]);
    steno_debug_ln("  attr: glue: %u", attr.glue);
    steno_debug("  output: '");
#endif

    uint8_t valid_len = 1, str_len = 0;
    uint8_t set_case;
    uint8_t *entry = entry_buf + 3 * strokes_len + 2;
    for (uint8_t i = 0; i < entry_len; i++) {
        // Commands
        set_case = 0;
        if (entry[i] < 32) {
#ifdef STENO_DEBUG_HIST
            steno_debug("'\n    ");
#endif
            switch (entry[i]) {
            case 0: // raw bytes of "length"
                space = 0;
                uint8_t len = entry[i + 1];
                uint8_t keycode_len = steno_send_keycodes(entry + i + 2, len);
                if (keycode_len > 0) {
                    str_len += keycode_len;
                } else {
                    valid_len = 0;
                }
                i += len + 1;   // Not + 2 because of increment at the end of the loop
                break;

            case 1: // lowercase next entry
                new_state.cap = CAPS_LOWER;
#ifdef STENO_DEBUG_HIST
                steno_debug_ln("LOWER");
#endif
                break;

            // TODO handle keep cases at the end of entry
            case 2: // Uppercase next entry
                new_state.cap = CAPS_UPPER;
#ifdef STENO_DEBUG_HIST
                steno_debug_ln("UPPER");
#endif
                set_case = 1;
                break;

            case 3: // capitalize next entry
                new_state.cap = CAPS_CAP;
#ifdef STENO_DEBUG_HIST
                steno_debug_ln("CAP");
#endif
                set_case = 1;
                break;

            case 4:; // keep case after "length" amount of characters
                uint8_t length = entry[i + 1];
#ifdef STENO_DEBUG_HIST
                steno_debug_ln("KEEP(%u)", length);
#endif
                for (int j = 0; j < length; j++) {
                    if (entry[j] >= 32 && entry[j] <= 127) {
                        steno_send_char(entry[j]);
                        i++;
                        str_len++;
                    } else if (entry[i] >= 128) {
                        int32_t code_point = 0;
                        const char *str = decode_utf8((char *) &entry[i], &code_point);
                        if (code_point > 0) {
                            str_len += steno_send_unicode(code_point);
                        }
                        i += str - (char *) &entry[i];
                    }
                }
                new_state.cap = old_state.cap;
                set_case = 1;
                break;

            case 5: // reset formatting
                new_state.cap = CAPS_LOWER;
                break;

            case 16: // add translation
                if (editing_state != ED_IDLE) {
                    dict_edit_puts("{add_trans}");
                    str_len += 11;
                } else {
                    dicted_add_prompt_strokes();
                }
                break;

            case 17: //edit translation
                if (editing_state == ED_ACTIVE_EDIT_TRANS) {
                    dict_edit_puts("{edit_trans}");
                    str_len += 10;
                } else {
                    dicted_edit_prompt_strokes();
                }
                break;

            case 18: //remove translation
                if (editing_state != ED_IDLE) {
                    dict_edit_puts("{rm_trans}");
                    str_len += 10;
                } else {
                    dicted_remove_prompt_strokes();
                }
                break;

            default:
                steno_error_ln("\nInvalid command: %X", entry[i]);
                return new_state;
            }
#ifdef STENO_DEBUG_HIST
            steno_debug("    '");
#endif
        } else if (entry[i] < 127) {
            if (space) {
                str_len++;
                steno_send_char(' ');
                space = 0;
            }
            switch (new_state.cap) {
            case CAPS_LOWER:
                steno_send_char(entry[i]);
                break;
            case CAPS_CAP:
                steno_send_char(toupper(entry[i]));
                new_state.cap = CAPS_LOWER;
                break;
            case CAPS_UPPER:
                steno_send_char(toupper(entry[i]));
                if (entry[i] == ' ') {
                    new_state.cap = CAPS_LOWER;
                }
                break;
            }
            str_len++;
            // Unicode
        } else {
            int32_t code_point = 0;
            const char *str = decode_utf8((char *) &entry[i], &code_point);
            if (code_point > 0) {
                steno_send_unicode(code_point);
            }
            str_len = 1;
            i = str - (char *) entry;
        }
    }
    if (!set_case) {
        new_state.cap = CAPS_LOWER;
    }

#ifdef STENO_DEBUG_HIST
    steno_debug_ln("'");
    steno_debug_ln("  -> %u", str_len);
#endif
    hist->len = valid_len ? str_len : 0;
    return new_state;
}
