#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include "orthography.h"
#include "mregexp.h"

#define sizeof_array(a) (sizeof(a) / sizeof(a[0]))
#define SEP " # "

typedef struct {
    MRegexp *word;
    /* const regex_t *suffix; */
    MRegexp *full;
    const char *word_re;
    const char *suffix_re;
    const char *repl;
} regex_rule_t;

#define CBMC

#include "ortho_test_data.c"
#ifdef CBMC
#define CBMC_assert(e, s) __CPROVER_assert(e, s)
#else
#define CBMC_assert(e, s)
#endif

static int replace(const char *restrict word, const char *restrict suffix, MRegexp *restrict full, const char *restrict repl, char *restrict output) {
    const int word_len = strlen(word), suffix_len = strlen(suffix);
#ifdef CBMC
    __CPROVER_assume(word_len <= 34 && suffix_len <= 10);
#endif
    char combined[word_len + suffix_len + 8];
    memset(combined, 0, word_len + suffix_len + 8);
    strcat(combined, word);
    strcat(combined, SEP);
    strcat(combined, suffix);
    MRegexpMatch m;
    if (mregexp_match(full, combined, &m)) {
        strncat(output, combined, m.match_begin);
        const int repl_len = strlen(repl);
#ifdef CBMC
        __CPROVER_assume(repl_len < 6);
#endif
        for (int i = 0; i < repl_len; i ++) {
            const char c = repl[i];
            if (c < ' ') {
                const MRegexpMatch *cap = mregexp_capture(full, c - 1);
                strncat(output, combined + cap->match_begin, cap->match_end - cap->match_begin);
            } else {
                output[strlen(output)] = c;
            }
        }
        return 0;
    } else {
        return -1;
    }
}

static int apply_rules_re(const regex_rule_t *rules, const char *restrict word, const char *restrict suffix, char *restrict output) {
    for (int i = 0; i < sizeof_array(RULES); i ++) {
        MRegexpMatch m;
        if (mregexp_match(rules[i].word, word, &m)) {
            int r = replace(word, suffix, rules[i].full, rules[i].repl, output);
            if (r < 0) {
                continue;
            }
            return i;
        }
    }
    return -1;
}

static int apply_rules_emb(const char *restrict word, const char *restrict suffix, char *restrict output, const bool chop) {
    const int word_len = strlen(word);
    const int word_start = word_len > 7 ? word_len - 7 : 0;
    char sub_output[16] = {0};
    int clause = 0;
    int ret = regex_ortho(word, suffix, sub_output, &clause);
    if (ret < 0) {
        return -1;
    } else {
        strncat(output, word, word_len - ret);
        strcat(output, sub_output);
        return clause;
    }
}

uint8_t ortho_rules(char *inp, char *output);
static int apply_rules_gen(const char *restrict word, const char *restrict suffix, char *restrict output) {
    const int word_len = strlen(word), suffix_len = strlen(suffix);
    char sub_output[64] = {0};
    char combined[word_len + suffix_len + 8];
    memset(combined, 0, word_len + suffix_len + 8);
    strcat(combined, word);
    strcat(combined, " ");
    strcat(combined, suffix);
    return ortho_rules(combined, sub_output);
}

static int diff_result(const regex_rule_t *rules, const char *restrict word, const char *restrict suffix) {
    /* char re_output[128] = {0}; */
    /* const int re_ret = apply_rules_re(rules, word, suffix, re_output); */
    /* const bool re_outted = re_ret > 0; */

    char emb_output[128] = {0};
    const int emb_ret = apply_rules_emb(word, suffix, emb_output, false);
    const bool emb_outted = emb_ret > 0;

    char gen_output[128] = {0};
    const int gen_ret = apply_rules_gen(word, suffix, gen_output);
    const bool gen_outted = gen_ret > 0;
#ifdef CBMC
    CBMC_assert(emb_outted == gen_outted, "rules ret");
    CBMC_assert(0 == strcmp(emb_output, gen_output), "rules output");
#else
    const bool ret_neq = emb_outted != gen_outted, out_neq = 0 != strcmp(emb_output, gen_output);
    if (ret_neq || out_neq) {
        if (ret_neq) {
            fprintf(stderr, "ret");
        } else {
            fprintf(stderr, "out");
        }
        fprintf(stderr, ": (%s %s) %s (%d) %s (%d)\n", word, suffix, emb_outted ? emb_output : "(none)", emb_ret, gen_outted ? gen_output : "(none)", gen_ret);
        return -1;
    }
#endif
    return 0;
}

#ifdef CBMC
static char *nondet_string();
#endif

int main() {
    regex_rule_t *rules = calloc(sizeof(regex_rule_t), sizeof_array(RULES));

    for (int i = 0; i < sizeof_array(RULES); i ++) {
        rules[i].word = mregexp_compile(RULES[i][0]);
        if (mregexp_error() != MREGEXP_OK) {
            fprintf(stderr, "compile: word: %d\n", mregexp_error());
            break;
        }

        int full_re_len = strlen(RULES[i][0]) + strlen(RULES[i][1]) + strlen(SEP);
        char full[full_re_len + 1];
        memset(full, 0, full_re_len + 1);
        strcat(full, RULES[i][0]);
        strcat(full, SEP);
        strcat(full, RULES[i][1]);
        rules[i].full = mregexp_compile(full);
        if (mregexp_error() != MREGEXP_OK) {
            fprintf(stderr, "compile: full: %d\n", mregexp_error());
            break;
        }
        rules[i].word_re = RULES[i][0];
        rules[i].suffix_re = RULES[i][1];
        rules[i].repl = RULES[i][2];
    }

/* #ifdef CBMC */
/*     const char *word = nondet_string(), *suffix = nondet_string(); */
/*     __CPROVER_assume(word != NULL && __CPROVER_is_fresh(word, 32) && strlen(word) < 32); */
/*     __CPROVER_assume(suffix != NULL && __CPROVER_is_fresh(suffix, 32) && strlen(suffix) < 32); */

/*     diff_result(rules, word, suffix); */
/* #else */
    for (int i = 0; i < sizeof_array(SUFFIXES); i ++) {
        for (int j = 0; j < sizeof_array(WORDS); j ++) {
            if (diff_result(rules, WORDS[j], SUFFIXES[i]) < 0) {
                return 1;
            }
        }
    }
/* #endif */
}
