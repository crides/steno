#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <malloc.h>
#include <oniguruma.h>
#include <assert.h>
#include "orthography.h"

#define sizeof_array(a) (sizeof(a) / sizeof(a[0]))
#define U_FFFD "\xef\xbf\xbd"

typedef struct {
    const regex_t *word;
    /* const regex_t *suffix; */
    const regex_t *full;
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

static int replace(const char *restrict word, const char *restrict suffix, const regex_t *restrict full, const char *restrict repl, char *restrict output) {
    OnigRegion *region = onig_region_new();
    const int word_len = strlen(word), suffix_len = strlen(suffix);
#ifdef CBMC
    __CPROVER_assume(word_len <= 34 && suffix_len <= 10);
#endif
    char combined[word_len + suffix_len + 8];
    memset(combined, 0, word_len + suffix_len + 8);
    strcat(combined, word);
    strcat(combined, U_FFFD);
    strcat(combined, suffix);
    const char *combined_end = combined + word_len + strlen(U_FFFD) + suffix_len;
    int ret = onig_search(full, (const UChar *) combined, (const UChar *) combined_end,
            (const UChar *) combined, (const UChar *) combined_end, region, ONIG_OPTION_NONE);
    if (ret > 0) {
        strncat(output, combined, region->beg[0]);
        const int repl_len = strlen(repl);
#ifdef CBMC
        __CPROVER_assume(repl_len < 6);
#endif
        for (int i = 0; i < repl_len; i ++) {
            const char c = repl[i];
            if (c < ' ') {
                strncat(output, combined + region->beg[c], region->end[c] - region->beg[c]);
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
    OnigRegion *region = onig_region_new();
    const UChar *start = word, *end = word + strlen(word);
    for (int i = 0; i < sizeof_array(RULES); i ++) {
        int ret = onig_search(rules[i].word, start, end, start, end, region, ONIG_OPTION_NONE);
        if (ret > 0) {
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

static int diff_result(const regex_rule_t *rules, const char *restrict word, const char *restrict suffix) {
    char re_output[128] = {0}, emb_output[128] = {0};
    const int re_ret = apply_rules_re(rules, word, suffix, re_output), emb_ret = apply_rules_emb(word, suffix, emb_output, false);
    const bool re_outted = re_ret > 0, emb_outted = emb_ret > 0;
#ifdef CBMC
    CBMC_assert(re_outted == emb_outted, "rules ret");
    CBMC_assert(0 == strcmp(re_output, emb_output), "rules output");
#else
    const bool ret_neq = re_outted != emb_outted, out_neq = 0 != strcmp(re_output, emb_output);
    if (ret_neq || out_neq) {
        if (ret_neq) {
            fprintf(stderr, "ret");
        } else {
            fprintf(stderr, "out");
        }
        fprintf(stderr, ": (%s %s) %s (%d) %s (%d)\n", word, suffix, re_outted ? re_output : "(none)", re_ret, emb_outted ? emb_output : "(none)", emb_ret);
        return -1;
    }
#endif
    return 0;
}

static char *nondet_string();

int main() {
    OnigOptionType options = ONIG_OPTION_CAPTURE_GROUP;
    regex_rule_t *rules = calloc(sizeof(regex_rule_t), sizeof_array(RULES));
    OnigErrorInfo einfo;

    for (int i = 0; i < sizeof_array(RULES); i ++) {
        int ret = onig_new(&rules[i].word, (const UChar *) RULES[i][0], (const UChar *) RULES[i][0] + strlen(RULES[i][0]),
                options, ONIG_ENCODING_UTF8, ONIG_SYNTAX_PERL_NG, &einfo);
        if (ret != ONIG_NORMAL) {
            fprintf(stderr, "onig_new: word: %d\n", ret);
            break;
        }
        /* ret = onig_new(&rules[i].suffix, (const UChar *) RULES[i][1], (const UChar *) RULES[i][1] + strlen(RULES[i][1]), */
        /*         options, ONIG_ENCODING_UTF8, ONIG_SYNTAX_PERL_NG, &einfo); */
        /* if (ret != ONIG_NORMAL) { */
        /*     fprintf(stderr, "onig_new: suffix: %d\n", ret); */
        /*     break; */
        /* } */

        int full_re_len = strlen(RULES[i][0]) + strlen(RULES[i][1]) + strlen(U_FFFD);
        char full[full_re_len + 1];
        memset(full, 0, full_re_len + 1);
        strcat(full, RULES[i][0]);
        strcat(full, U_FFFD);
        strcat(full, RULES[i][1]);
        ret = onig_new(&rules[i].full, (const UChar *) full, (const UChar *) full + full_re_len,
                options, ONIG_ENCODING_UTF8, ONIG_SYNTAX_PERL_NG, &einfo);
        if (ret != ONIG_NORMAL) {
            fprintf(stderr, "onig_new: full: %d\n", ret);
            break;
        }
        rules[i].word_re = RULES[i][0];
        rules[i].suffix_re = RULES[i][1];
        rules[i].repl = RULES[i][2];
    }

#ifdef CBMC
    const char *word = nondet_string(), *suffix = nondet_string();
    __CPROVER_assume(word != NULL && __CPROVER_is_fresh(word, 32) && strlen(word) < 32);
    __CPROVER_assume(suffix != NULL && __CPROVER_is_fresh(suffix, 32) && strlen(suffix) < 32);

    diff_result(rules, word, suffix);
#else
    for (int i = 0; i < sizeof_array(SUFFIXES); i ++) {
        for (int j = 0; j < sizeof_array(WORDS); j ++) {
            if (diff_result(rules, WORDS[j], SUFFIXES[i]) < 0) {
                return 1;
            }
        }
    }
#endif
}
