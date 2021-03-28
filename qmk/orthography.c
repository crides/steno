#include "steno.h"
#include "orthography.h"
#include <string.h>
#include <ctype.h>

static bool strneq(const char *a, const char *b, const uint8_t n) {
    return strncmp(a, b, n) == 0;
}

static bool chrin(const char *s, const char c) {
    // strchr("", 0) != NULL
    return c != 0 && strchr(s, c);
}

// Returns how many chars to backspace, and what text (`output`) to append after
// NOTE assumes the suffix we get is valid, so no end of string checking
int8_t ortho_transform(const char *word, const char *suffix, char *output) {
    const uint8_t word_len = strlen(word);
    char rev[WORD_ENDING_SIZE];
    // Invert `word` so that indexing is anchored to the end
    /* steno_debug("rev: "); */
    for (uint8_t i = 0; i < WORD_ENDING_SIZE; i ++) {
        if (word_len >= i) {
            rev[i] = word[word_len - i - 1];
            /* steno_debug("%c", rev[i]); */
        }
    }
    /* steno_debug_ln(""); */
    /* steno_debug_ln("suffix: %02X%02X%02X%02X", suffix[0], suffix[1], suffix[2], suffix[3]); */

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
    //  + s(\w|$) -> \1es\2
    // ([bcdfghjklmnpqrstvwxz])y + s(\w|$) -> \1ies\2
    if (suffix[0] == 's') {
        if ((chrin("sxz", rev[0]) || (rev[1] == 'h' && (rev[2] == 's' || rev[2] == 'z')))
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

    // XXX ((?:^|\\W)(?:[bcdfghjklmnprstvwxyz]+|[bcdfghjklmnprstvwxyz]*qu)[aeiou])([bdfgklmnprstz]) + (ed|en|er|ier|est|ing|y|ie|ies|iest|iness|ish|abl[ey]|ability|abilities) -> \1\2\2\3
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
                || (rev[1] == 'u' && chrin("gbdlmntv", rev[0]))) {
            bool junk = chrin("bcdfghjklmnprstvwxyz", rev[2]);
            if (junk || (rev[1] != 'u' && strneq("uq", rev + 2, 2))) {
                output[0] = rev[0];
                strcat(output, suffix);
                return 0;
            }
        }
    }

    return -1;
}
