// History management and output control
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>

#include "hist.h"
#include "steno.h"
#include "store.h"
#ifdef QMK_KEYBOARD
#include "process_keycode/process_unicode_common.h"
#endif
#ifndef STENO_READONLY
#include "dict_editing.h"
#endif
#ifndef STENO_NOORTHOGRAPHY
#include "orthography.h"
#endif
#ifndef STENO_NOUI
#include "disp.h"
#endif
#ifdef CONFIG_ZMK_KEYBOARD_NAME
#include "dt-bindings/zmk/keys.h"
#endif

static history_t history[HIST_SIZE];

extern char last_trans[128];
extern uint8_t last_trans_size;

#ifndef STENO_READONLY
static void dict_edit_puts(const char *const str) {
    const uint16_t len = strlen(str);
    if (len + (uint16_t) entry_buf_len > 255) {
        return;
    }
    memcpy(entry_buf + entry_buf_len, str, len);
    entry_buf_len += len;
    disp_trans_edit_handle_str(str);
}
#endif

static void steno_back(const uint8_t len) {
#ifndef STENO_READONLY
    if (editing_state != ED_IDLE) {
        if (entry_buf_len > len) {
            disp_trans_edit_back(len);
            entry_buf_len -= len;
        } else {
            disp_trans_edit_back(entry_buf_len);
            entry_buf_len = 0;
        }
    } else
#endif
    {
        for (uint8_t i = 0; i < len; i ++) {
            tap_code(
#if defined(QMK_KEYBOARD)
                    KC_BSPC
#elif defined(CONFIG_ZMK_KEYBOARD_NAME)
                    BSPC
#endif
                    );
        }
    }
}

static void steno_send_char(const char c) {
#ifndef STENO_READONLY
    if (editing_state != ED_IDLE) {
        if (entry_buf_len < 255) {
            entry_buf[entry_buf_len ++] = c;
            disp_trans_edit_handle_char(c);
        }
    } else
#endif
    {
        if (last_trans_size < 128) {
            last_trans[last_trans_size++] = c;
        }
        send_char(c);
    }
#ifdef STENO_DEBUG_HIST
    steno_debug("%c", c);
#endif
}

#ifndef STENO_NOUNICODE
static uint8_t steno_send_unicode(const uint32_t u) {
#ifdef STENO_DEBUG_HIST
    if (u < 0xFFFF) {
        steno_debug("\\u" DWF("04"), u);
    } else {
        steno_debug("\\U" DWF("06"), u);
    }
#endif
    char buf[16];
    uint8_t len;
    if (u < 0xFFFF) {
        snprintf(buf, 16, "\\u" DWF("04"), u);
        len = 6;
    } else {
        snprintf(buf, 16, "\\U" DWF("06"), u);
        len = 8;
    }
#ifndef STENO_READONLY
    if (editing_state != ED_IDLE) {
        dict_edit_puts(buf);
        return len;
    } else
#endif
    {
        if (last_trans_size + len < 128) {
            for (uint8_t i = 0; i < len; i ++) {
                last_trans[last_trans_size++] = buf[i];
            }
        }
        register_unicode(u);
        return 1;
    }
}
#endif

static uint8_t steno_send_keycodes(const uint8_t *const keycodes, const uint8_t len) {
#ifdef STENO_DEBUG_HIST
    steno_debug("keys(%u):", len);
#endif
    uint8_t mods = 0;
#ifndef STENO_READONLY
    char buf[16];
    uint8_t output_len = 3;
    if (editing_state != ED_IDLE) {
        dict_edit_puts("\\k[");
    }
#endif
    for (uint8_t i = 0; i < len; i++) {
        if ((keycodes[i] & 0xF8) == 0xE0) {
            const uint8_t mod = keycodes[i] & 0x07;
#if defined(STENO_DEBUG_HIST) || !defined(STENO_READONLY)
            uint8_t mod_char;
            switch (mod & 3) {
            case 0: mod_char = 'c'; break;
            case 1: mod_char = 's'; break;
            case 2: mod_char = 'a'; break;
            case 3: mod_char = 'g'; break;
            }
#endif
            const uint8_t mod_mask = 1 << mod;
            if (mods & mod_mask) {
#ifndef STENO_READONLY
                if (editing_state != ED_IDLE) {
                    dict_edit_puts(")");
                    output_len += 1;
                } else
#endif
                {
                    unregister_code(keycodes[i]);
                }
#ifdef STENO_DEBUG_HIST
                steno_debug(" %c", mod_char);
#endif
            } else {
#ifndef STENO_READONLY
                if (editing_state != ED_IDLE) {
                    snprintf(buf, 16, "\\%c(", mod_char);
                    dict_edit_puts(buf);
                    output_len += 3;
                } else
#endif
                {
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
#ifndef STENO_READONLY
            if (editing_state != ED_IDLE) {
                snprintf(buf, 16, " %02X", keycodes[i]);
                dict_edit_puts(buf);
                output_len += 3;
            } else
#endif
            {
                tap_code(keycodes[i]);
            }
        }
    }
#ifdef STENO_DEBUG_HIST
    steno_debug("\n");
#endif
#ifndef STENO_READONLY
    if (editing_state != ED_IDLE) {
        dict_edit_puts("]");
        return output_len + 1;
    } else
#endif
    {
        return 0;
    }
}

inline history_t *hist_get(const uint8_t ind) {
    return &history[ind];
}

// Undo the last history entry. First delete the output, and then start from the initial state of the
// multi-stage input, and rebuild the output from there.
void hist_undo(const uint8_t h_ind) {
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("hist_undo(%u)", h_ind);
#endif
    history_t *const hist = hist_get(h_ind);
    const uint8_t len = hist->len;
    if (!len) {
        steno_error_ln("bad cur hist entry");
        steno_back(1);
        return;
    }

#ifdef STENO_DEBUG_HIST
    steno_debug_ln("  back: %u", len);
#endif
    steno_back(len);
    const uint8_t strokes_len =
#ifdef STENO_NOORTHOGRAPHY
            BUCKET_GET_STROKES_LEN(hist->bucket);
#else
            hist->ortho_len;
#endif
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("  strokes: %u", strokes_len);
#endif
    const uint8_t repl_len = strokes_len > 1 ? strokes_len - 1 : 0;
    for (uint8_t i = 0; i < repl_len; i++) {
        const uint8_t old_hist_ind = HIST_LIMIT(h_ind + i - repl_len);
        const history_t *const old_hist = hist_get(old_hist_ind);
#ifdef STENO_DEBUG_HIST
        steno_debug_ln("  old_hist_ind: %u", old_hist_ind);
#endif
        if (!old_hist->len) {
            hist->len = 0;
            steno_error_ln("bad prev hist entry");
            return;
        }
        process_output(old_hist_ind);
    }
    return;
}

// Process the output. If it's a raw stroke (no nodes found for the input), then just output the stroke;
// otherwise, load the entry, perform the necessary transformations for capitalization, and output according
// to the bytes. Also takes care of outputting key codes and Unicode characters.
state_t process_output(const uint8_t h_ind) {
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("proc_out(%u)", h_ind);
#endif
    // TODO optimization: compare beginning of current and string to replace
    history_t *const hist = hist_get(h_ind);
    const state_t old_state = hist->state;
    state_t new_state = old_state;
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("  old scg: %u%u%u", old_state.space, old_state.cap, old_state.glue);
#endif

    if (hist->bucket == 0) {
#ifdef STENO_DEBUG_HIST
        steno_debug_ln("  stroke: " DWF("06"), hist->stroke & 0xFFFFFF);
#endif
        char buf[24];
        new_state.space = 1;
        new_state.cap = 0;
        if (stroke_to_string(hist->stroke, buf, &hist->len)) {
            new_state.glue = 1;
        }
#ifndef STENO_NOORTHOGRAPHY
        hist->ortho_len = 1;
#endif
#ifdef STENO_DEBUG_HIST
        steno_debug("  out: '");
#endif
        if (old_state.space && !(old_state.glue && new_state.glue)) {
            steno_send_char(' ');
            hist->len++;
        }
        for (char *str = buf; *str; str ++) {
            steno_send_char(*str);
        }
#ifdef STENO_DEBUG_HIST
        steno_debug_ln("'\n  -> %u", hist->len);
#endif
        return new_state;
    }

    const uint32_t bucket = hist->bucket;
    memset(kvpair_buf, 0, 128);      // TODO optimize
    read_entry(bucket, kvpair_buf);
    const uint8_t entry_len = BUCKET_GET_ENTRY_LEN(bucket);
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("  entry_len: %u", entry_len);
#endif

    const uint8_t strokes_len = BUCKET_GET_STROKES_LEN(hist->bucket);
    const attr_t attr = *((attr_t *) &kvpair_buf[strokes_len * STROKE_SIZE]);
    new_state.space = attr.space_after;
    new_state.glue = attr.glue;
    uint8_t space = old_state.space && attr.space_prev && entry_len && !(old_state.glue && attr.glue);
#ifdef STENO_DEBUG_HIST
    steno_debug_ln("  attr: prev, glue, after: %u%u%u", attr.space_prev, attr.glue, attr.space_after);
    steno_debug("  output: '");
#endif
    const uint8_t *entry = kvpair_buf + STROKE_SIZE * strokes_len + 1;

#ifndef STENO_NOORTHOGRAPHY
    // Possible suffix
    if (!attr.space_prev && strokes_len == 1) {
        const uint8_t last_hist_ind = HIST_LIMIT(h_ind - strokes_len);
        const history_t *const last_hist = hist_get(last_hist_ind);
        char word_end[32];
        memcpy(word_end, last_hist->end_buf, 8);
        char output[16] = {0};
        // NOTE assuming everything is ascii i.e. no commands, unicode, keycodes
        const int8_t ret = process_ortho((const char *) word_end, (const char *) entry, output);
        if (ret >= 0) {
            const uint8_t output_len = strlen(output);
            const uint8_t old_end_len = strlen((const char *) word_end);
            const uint8_t output_total_len = old_end_len - ret + output_len;
            memcpy(word_end + old_end_len - ret, output, output_len);
            word_end[output_total_len] = 0;
            const uint8_t start_of_end = output_total_len < 7 ? 0 : output_total_len - 7;
            strncpy((char *) hist->end_buf, (const char *) word_end + start_of_end, 7);
            hist->end_buf[7] = 0;
            if (ret > 0) {
                hist->len = last_hist->len - ret + output_len;
                hist->ortho_len = strokes_len + last_hist->ortho_len;
            } else {    // Optimization: reduce down to 1 stroke/entry cuz no part of original is taken away
                hist->len = output_len;
                hist->ortho_len = strokes_len;
            }
#ifdef STENO_DEBUG_HIST
            steno_debug_ln("  ortho_len: %u", hist->ortho_len);
#endif
            steno_back(ret);
            for (uint8_t i = 0; i < 32 && output[i]; i ++) {
                steno_send_char(output[i]);
            }
            return new_state;
        }
    }
    const uint8_t start_of_end = entry_len < 7 ? 0 : entry_len - 7;
    strncpy((char *) hist->end_buf, (const char *) entry + start_of_end, 7);
    hist->end_buf[7] = 0;
    hist->ortho_len = strokes_len;
#endif

    {
        const uint8_t repl_len = strokes_len > 1 ? strokes_len - 1 : 0;
        for (int8_t counter = repl_len; counter > 0; ) {
            const uint8_t old_hist_ind = HIST_LIMIT(h_ind + counter - repl_len - 1);
            const history_t *const old_hist = hist_get(old_hist_ind);
#ifdef STENO_DEBUG_HIST
            steno_debug_ln("  old_hist_ind: %u, bspc len: %u", old_hist_ind, old_hist->len);
#endif
            if (!old_hist->len) {
                hist->len = 0;
                steno_error_ln("bad prev hist entry");
                break;
            }
            steno_back(old_hist->len);
            const uint8_t old_strokes_len = BUCKET_GET_STROKES_LEN(old_hist->bucket);
#ifdef STENO_NOORTHOGRAPHY
            const uint8_t old_repl_len = old_strokes_len > 1 ? old_strokes_len - 1 : 0;
            counter -= old_repl_len + 1;
#else
            const uint8_t old_ortho_len = old_hist->ortho_len;
            if (counter >= old_ortho_len) {
                counter -= old_ortho_len;
            } else if (old_ortho_len != old_strokes_len && counter == old_strokes_len) {
                const uint8_t old_word_stroke_len = old_ortho_len - old_strokes_len;
                for (uint8_t i = 0; i < old_word_stroke_len; i++) {
                    const uint8_t old_hist_ind = HIST_LIMIT(h_ind + i - old_ortho_len);
                    const history_t *const old_hist = hist_get(old_hist_ind);
#ifdef STENO_DEBUG_HIST
                    steno_debug_ln("  old_hist_ind: %u", old_hist_ind);
#endif
                    if (!old_hist->len) {
                        hist->len = 0;
                        steno_error_ln("bad prev hist entry");
                        return new_state;
                    }
                    process_output(old_hist_ind);
                }
                // Reread the bucket cuz buffer is used
                read_entry(bucket, kvpair_buf);
                counter -= old_strokes_len;
            } else {
                steno_error_ln("??? ortho %u strokes %u counter %u", old_ortho_len, old_strokes_len, counter);
            }
#endif
        }
    }

    uint8_t valid_len = 1, str_len = 0;
    uint8_t set_case;
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
                const uint8_t len = entry[i + 1];
                const uint8_t keycode_len = steno_send_keycodes(entry + i + 2, len);
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
                set_case = 1;
                break;

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
                const uint8_t length = entry[++i];
#ifdef STENO_DEBUG_HIST
                steno_debug_ln("KEEP(%u)", length);
#endif
                i ++;
                const uint8_t end = length + i;
                if (space) {
                    str_len++;
                    steno_send_char(' ');
                    space = 0;
                }
                for ( ; i < end; i++) {
                    if (entry[i] >= 32 && entry[i] <= 127) {
                        steno_send_char(entry[i]);
                        str_len++;
                    } else if (entry[i] >= 128) {
                        int32_t code_point = 0;
                        const char *str = decode_utf8((char *) &entry[i], &code_point);
#ifndef STENO_NOUNICODE
                        if (code_point > 0) {
                            str_len += steno_send_unicode(code_point);
                        }
#endif
                        i += str - (char *) &entry[i];
                    }
                }
                new_state.cap = old_state.cap;
                set_case = 1;
                break;

            case 5: // reset formatting
                new_state.cap = CAPS_NORMAL;
                break;

#ifndef STENO_READONLY
            case 16: // Dictionary editing
                if (editing_state != ED_IDLE) {
                    dict_edit_puts("{dicted}");
                    str_len += 8;
                } else {
                    dicted_update();
                }
                break;
#endif

            default:
                steno_error_ln("\nInvalid cmd: %X", entry[i]);
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
            case CAPS_NORMAL:
                steno_send_char(entry[i]);
                break;
            case CAPS_LOWER:
                steno_send_char(tolower(entry[i]));
                if (entry[i] == ' ') {
                    new_state.cap = CAPS_NORMAL;
                }
                break;
            case CAPS_CAP:
                steno_send_char(toupper(entry[i]));
                new_state.cap = CAPS_NORMAL;
                break;
            case CAPS_UPPER:
                steno_send_char(toupper(entry[i]));
                if (entry[i] == ' ') {
                    new_state.cap = CAPS_NORMAL;
                }
                break;
            }
            str_len++;
            // Unicode
        } else {
            int32_t code_point = 0;
            const char *str = decode_utf8((char *) &entry[i], &code_point);
#ifndef STENO_NOUNICODE
            if (code_point > 0) {
                steno_send_unicode(code_point);
            }
#endif
            str_len += 1;
            i = str - (char *) entry;
        }
    }
    if (!set_case) {
        new_state.cap = CAPS_NORMAL;
    }

#ifdef STENO_DEBUG_HIST
    steno_debug_ln("'");
    steno_debug_ln("  -> %u", str_len);
#endif
    hist->len = valid_len ? str_len : 0;
    return new_state;
}
