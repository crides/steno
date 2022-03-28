#include "steno.h"
#include "store.h"
#include "orthography.h"
#include <string.h>
#include <ctype.h>

static bool strneq(const char *const a, const char *const b, const uint8_t n) {
    return strncmp(a, b, n) == 0;
}

static bool chrin(const char *const s, const char c) {
    return c != 0 && strchr(s, c);
}

// Returns how many chars to backspace, and what text (`output`) to append after
// NOTE assumes the suffix we get is valid, so no end of string checking
// Rules borrowed from https://github.com/nimble0/dotterel/blob/master/app/src/main/assets/orthography/english.regex.json
static int8_t regex_ortho(const char *const word, const char *const suffix, char *const output) {
    const uint8_t word_len = strlen(word);
    char rev[WORD_ENDING_SIZE];
    // Invert `word` so that indexing is anchored to the end
    for (uint8_t i = 0; i < WORD_ENDING_SIZE; i ++) {
        if (word_len >= i) {
            rev[i] = word[word_len - i - 1];
        }
    }

    // { ([aeiou]c)$ or (ion)$ or (.*ur)e } + ly -> \1ally
    if (strneq(suffix, "ly", 2)) {
        if ((rev[0] == 'c' && chrin("aeiou", rev[1])) || strneq(rev, "noi", 3)) {
            strcat(output, "ally");
            strcat(output, suffix + 2);
            return 0;
        }
        if (strneq(rev, "eru", 3)) {
            strcat(output, "ally");
            strcat(output, suffix + 2);
            return 1;
        }
    }
    
    // (t)e + (ry|ries) -> \1o\2
    if (strneq("et", rev, 2) && (strneq("ry", suffix, 2) || strneq("ries", suffix, 4))) {
        output[0] = 'o';
        strcat(output, suffix);
        return 1;
    }

    // ([naeiou])te? + (cy|cies) -> \1\2
    if (strneq("cy", suffix, 2) || strneq("cies", suffix, 4)) {
        const uint8_t t_start = rev[0] == 'e';
        if (rev[t_start] == 't' && chrin("naeiou", rev[t_start + 1])) {
            strcat(output, suffix);
            return t_start + 1;
        }
    }

    // word: (s|sh|x|z|zh)
    //       ((?:oa|ea|i|ee|oo|au|ou|l|n|(?<![gin]a)r|t)ch)
    //  + s([^\w]|$) -> \1es\2
    // ([bcdfghjklmnpqrstvwxz])y + s([^\w]|$) -> \1ies\2
    // End of string is just \0, which is not [[:alpha:]]
    if (suffix[0] == 's' && !isalpha(suffix[1])) {
        if ((chrin("sxz", rev[0]) || (rev[0] == 'h' && (rev[1] == 's' || rev[1] == 'z')))
                || (strneq("hc", rev, 2) && (chrin("ilnt", rev[2]) || strneq("ao", rev + 2, 2) || strneq("ae", rev + 2, 2)
                        || strneq("ee", rev + 2, 2) || strneq("oo", rev + 2, 2)
                        || strneq("ua", rev + 2, 2) || strneq("uo", rev + 2, 2)
                        || (rev[2] == 'r' && !(rev[3] == 'a' && chrin("gin", rev[4])))))) {
            output[0] = 'e';
            strcat(output, suffix);
            return 0;
        }
        if (rev[0] == 'y' && chrin("bcdfghjklmnpqrstvwxz", rev[1])) {
            output[0] = 'i'; output[1] = 'e';
            strcat(output, suffix);
            return 1;
        }
    }

    // (\w)ie + ing -> \1ying
    if (strneq("ei", rev, 2) && isalpha(rev[2]) && strneq("ing", suffix, 3)) {
        strcpy(output, "ying");
        return 2;
    }

    // (\w[cdfghlmnpr])y + ist -> \1ist
    if (strneq("ist", suffix, 3) && rev[0] == 'y' && chrin("cdfghlmnpr", rev[1])) {
        output[0] = rev[1];
        strcat(output, "ist");
        return 1;
    }

    // (\w[bcdfghjklmnpqrstvwxz])y + ([abcdefghjklnopqrstuxz]) -> \1i\2
    if (rev[0] == 'y' && chrin("bcdfghjklmnpqrstvwxz", rev[1]) && isalpha(rev[2]) && chrin("abcdefghjklnopqrstuxz", suffix[0])) {
        strcat(output, "i");
        strcat(output, suffix);
        return 1;
    }

    // word: (\w[^aeiou]it)
    //       (\wct)
    //       (\w[^aeiou]is)e
    //       (\w[aeiou][bcdfghjklmnprstvwxyz]+at)e
    //  + er(s?) -> \1or\2
    if (strneq("er", suffix, 2)) {
        if (rev[0] == 't' && (rev[1] == 'c' || (rev[1] == 'i' && !chrin("aeiou", rev[2])))) {
            output[0] = 'o';
            strcat(output, suffix + 1);
            return 0;
        }
        if (rev[0] == 'e') {
            if (rev[1] == 's' && rev[2] == 'i' && !chrin("aeiou", rev[3])) {
                output[0] = 'o';
                strcat(output, suffix + 1);
                return 1;
            }
            if (rev[1] == 't' && rev[2] == 'a' && chrin("bcdfghjklmnprstvwxyz", rev[3])) {
                uint8_t i = 4;
                for ( ; i < WORD_ENDING_SIZE && chrin("bcdfghjklmnprstvwxyz", rev[i]); i ++);
                if (chrin("aeiou", rev[i])) {
                    output[0] = 'o';
                    strcat(output, suffix + 1);
                    return 1;
                }
            }
        }
    }

    // (\w[bcdfghjklmnpqrstuvwxz])e + { ([aeiouy]\w) or ([aeoy]) } -> \1\2
    if (rev[0] == 'e' && chrin("bcdfghjklmnpqrstuvwxz", rev[1]) && isalpha(rev[2]) \
            && ((chrin("aeiouy", suffix[0]) && isalpha(suffix[1])) || chrin("aeoy", suffix[0]))) {
        strcat(output, suffix);
        return 1;
    }

    // ([aeiouy](?:pod|log)) + ([aeiouy]) -> \1\2
    if (chrin("aeiouy", rev[3]) && (strneq("gol", rev, 3) || strneq("dop", rev, 3)) && chrin("aeiouy", suffix[0])) {
        strcat(output, suffix);
        return 0;
    }

    // ((?:[bcdfghjklmnprstvwxyz]|qu)[ae]l) + y -> \1ly
    if (suffix[0] == 'y' && rev[0] == 'l' && (rev[1] == 'a' || rev[1] == 'e')
            && (strneq("uq", rev + 2, 2) || chrin("bcdfghjklmnprstvwxyz", rev[2]))) {
        output[0] = 'l'; output[1] = 'y';
        return 0;
    }

    // suffix: (ed|en|er|ier|est|in[g]|y|ie|ies|iest|iness|ish|abl[ey]|ability|abilities)
    // word: ((?:[bcdfghjklmnprstvwxyz]+|[bcdfghjklmnprstvwxyz]*qu)[aeiou])([bdfgklmnprstz])
    //       ((?:[bcdfghjklmnprstvwxyz]|qu)a)([gbmptv])
    //       ((?:[bcdfghjklmnprstvwxyz]|qu)e)([gbpv])
    //       ((?:[bcdfghjklmnprstvwxyz]|qu)i)([gbmpv])
    //       ((?:[bcdfghjklmnprstvwxyz]|qu)o)([gbdlv])
    //       ([bcdfghjklmnprstvwxyz]u)([gbdlmntv])
    // repl: \1\2\2\3
    if ((suffix[0] == 'e' && (chrin("dnr", suffix[1]) || strneq(suffix + 1, "st", 2))) \
            || (strneq(suffix, "ab", 2) && (suffix[2] == 'l' && chrin("ey", suffix[3]))) \
                || (strneq(suffix + 2, "ilit", 4) && (suffix[6] == 'y' || strneq(suffix + 6, "ies", 3))) \
            || (suffix[0] == 'i' && chrin("ne", suffix[1]))      // Omitting `ier`, `ies`, `iest`
            || strneq(suffix, "ish", 3)
            || suffix[0] == 'y') {
        if ((rev[1] == 'a' && chrin("gbmptv", rev[0])) || (rev[1] == 'e' && chrin("gbpv", rev[0]))
                || (rev[1] == 'i' && chrin("gbmpv", rev[0])) || (rev[1] == 'o' && chrin("gbdlv", rev[0]))
                || (rev[1] == 'u' && chrin("gbdlmntv", rev[0]))
                || (chrin("bdfgklmnprstz", rev[0]) && chrin("aeiou", rev[1]))) {
            const bool junk = chrin("bcdfghjklmnprstvwxyz", rev[2]);
            if (junk || (rev[1] != 'u' && strneq("uq", rev + 2, 2))) {
                output[0] = rev[0];
                strcat(output, suffix);
                return 0;
            }
        }
    }
    return -1;
}

#define ORTHO_BUCKET_SIZE 3
#define ORTHO_BUCKET_NUM 0x3C00
#define BUCKET_OFFSET_BITS 18
#define BUCKET_LENGTH_BITS 6

static uint32_t hash_str(const char *str) {
    uint32_t hash = FNV_SEED;
    while (*str) {
        hash *= FNV_FACTOR;
        hash ^= *str;
        str ++;
    }
    return hash;
}

static int8_t simple_ortho(const char *const word, const char *const suffix, char *const output) {
    const uint8_t word_len = strlen(word);
    const uint8_t suffix_len = strlen(suffix);
    if (word_len > 13 || suffix_len > 9) {  // None of the entries in the rules are that long
        return -1;
    }
    uint8_t merged[24] = {0};
    memcpy(merged, word, word_len);
    merged[word_len] = ' ';
    memcpy(merged + word_len + 1, suffix, suffix_len);
    const uint8_t merged_len = word_len + suffix_len + 1;
    const uint32_t hash = hash_str((const char *) merged);
    const uint32_t bucket_ind = hash % (ORTHO_BUCKET_NUM - 1);
    uint32_t bucket_addr = ORTHOGRAPHY_START + bucket_ind * ORTHO_BUCKET_SIZE;
    uint32_t bucket;

    const uint8_t length_mask = (1 << BUCKET_LENGTH_BITS) - 1;
    for (; ; bucket_addr += ORTHO_BUCKET_SIZE) {
        store_read(bucket_addr, (uint8_t *) &bucket, ORTHO_BUCKET_SIZE);
        const uint8_t entry_len = (bucket >> BUCKET_OFFSET_BITS) & length_mask;
        if (entry_len == length_mask) {
            return -1;
        }
        const uint32_t entry_addr = (bucket & (((uint32_t) 1 << BUCKET_OFFSET_BITS) - 1)) + (uint32_t) ORTHOGRAPHY_START + (uint32_t) ORTHO_BUCKET_SIZE * ORTHO_BUCKET_NUM;
        uint8_t entry_buf[32];
        store_read(entry_addr, entry_buf, entry_len);
        if (strcmp((const char *) merged, (const char *) entry_buf) == 0) {
            const uint8_t extra_len = entry_len - merged_len - 2;
            memcpy(output, entry_buf + merged_len + 2, extra_len);
            output[extra_len] = 0;
            return (int8_t) entry_buf[merged_len + 1];
        } else {
            continue;
        }
    }
    return -1;
}

int8_t process_ortho(const char *const word, const char *const suffix, char *const output) {
    const int8_t ret = regex_ortho(word, suffix, output);
    if (ret == -1) {
        return simple_ortho(word, suffix, output);
    }
    return ret;
}
