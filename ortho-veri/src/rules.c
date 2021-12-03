#include <string.h>
#include <stdint.h>
#include <stdbool.h>
typedef struct {
    uint8_t start, end;
} group_t;
uint8_t ortho_rules(char *inp, char *output) {
const uint8_t inp_len = strlen(inp); bool error = false; group_t groups[10] = {0};

        // ([aeiou]c) ly
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
do {
if (s[i] == 97) { i ++; continue; }
if (s[i] == 101) { i ++; continue; }
if (s[i] == 105) { i ++; continue; }
if (s[i] == 111) { i ++; continue; }
if (s[i] == 117) { i ++; continue; }
error = true; } while (0);
if (s[i] == 99) { i ++; } else { error = true; break; }
groups[1].end = i;
if (s[i] == 32) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
if (s[i] == 121) { i ++; } else { error = true; break; }
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strcat(output, "ally");
return j; } }

        // (ion) ly
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 111) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
groups[1].end = i;
if (s[i] == 32) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
if (s[i] == 121) { i ++; } else { error = true; break; }
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strcat(output, "ally");
return j; } }

        // (ur)e ly
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
if (s[i] == 117) { i ++; } else { error = true; break; }
if (s[i] == 114) { i ++; } else { error = true; break; }
groups[1].end = i;
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 32) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
if (s[i] == 121) { i ++; } else { error = true; break; }
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strcat(output, "ally");
return j; } }

        // (t)e (ry|ries)
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
if (s[i] == 116) { i ++; } else { error = true; break; }
groups[1].end = i;
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 32) { i ++; } else { error = true; break; }
groups[2].start = i;
error = false; i_save = i; do {
if (s[i] == 114) { i ++; } else { error = true; break; }
if (s[i] == 121) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 114) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
groups[2].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strcat(output, "o");
strncat(output, s, groups[2].end - groups[2].start);
return j; } }

        // ([naeiou])te? (cy|cies)
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
do {
if (s[i] == 97) { i ++; continue; }
if (s[i] == 101) { i ++; continue; }
if (s[i] == 105) { i ++; continue; }
if (110 <= s[i] && s[i] <= 111) { i ++; continue; }
if (s[i] == 117) { i ++; continue; }
error = true; } while (0);
groups[1].end = i;
if (s[i] == 116) { i ++; } else { error = true; break; }
{
if (s[i] == 101) { i ++; } else { error = true; break; }
error = false; }
if (s[i] == 32) { i ++; } else { error = true; break; }
groups[2].start = i;
error = false; i_save = i; do {
if (s[i] == 99) { i ++; } else { error = true; break; }
if (s[i] == 121) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 99) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
groups[2].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strncat(output, s, groups[2].end - groups[2].start);
return j; } }

        // (s|sh|x|z|zh) s([^\w]|$)
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
error = false; i_save = i; do {
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 104) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 120) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 122) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 122) { i ++; } else { error = true; break; }
if (s[i] == 104) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
groups[1].end = i;
if (s[i] == 32) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
groups[2].start = i;
error = false; i_save = i; do {
do {
if (0 <= s[i] && s[i] <= 47) { i ++; continue; }
if (58 <= s[i] && s[i] <= 64) { i ++; continue; }
if (91 <= s[i] && s[i] <= 94) { i ++; continue; }
if (s[i] == 96) { i ++; continue; }
if (123 <= s[i] && s[i] <= 255) { i ++; continue; }
error = true; } while (0);
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (i != s_len) { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
groups[2].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strcat(output, "es");
strncat(output, s, groups[2].end - groups[2].start);
return j; } }

        // ([bcdfghjklmnpqrstvwxz])y s([^\w]|$)
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
do {
if (98 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 110) { i ++; continue; }
if (112 <= s[i] && s[i] <= 116) { i ++; continue; }
if (118 <= s[i] && s[i] <= 120) { i ++; continue; }
if (s[i] == 122) { i ++; continue; }
error = true; } while (0);
groups[1].end = i;
if (s[i] == 121) { i ++; } else { error = true; break; }
if (s[i] == 32) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
groups[2].start = i;
error = false; i_save = i; do {
do {
if (0 <= s[i] && s[i] <= 47) { i ++; continue; }
if (58 <= s[i] && s[i] <= 64) { i ++; continue; }
if (91 <= s[i] && s[i] <= 94) { i ++; continue; }
if (s[i] == 96) { i ++; continue; }
if (123 <= s[i] && s[i] <= 255) { i ++; continue; }
error = true; } while (0);
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (i != s_len) { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
groups[2].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strcat(output, "ies");
strncat(output, s, groups[2].end - groups[2].start);
return j; } }

        // (\w)ie ing
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
do {
if (48 <= s[i] && s[i] <= 57) { i ++; continue; }
if (65 <= s[i] && s[i] <= 90) { i ++; continue; }
if (s[i] == 95) { i ++; continue; }
if (97 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
groups[1].end = i;
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 32) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
if (s[i] == 103) { i ++; } else { error = true; break; }
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strcat(output, "ying");
return j; } }

        // (\w[cdfghlmnpr])y ist
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
do {
if (48 <= s[i] && s[i] <= 57) { i ++; continue; }
if (65 <= s[i] && s[i] <= 90) { i ++; continue; }
if (s[i] == 95) { i ++; continue; }
if (97 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
do {
if (99 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (108 <= s[i] && s[i] <= 110) { i ++; continue; }
if (s[i] == 112) { i ++; continue; }
if (s[i] == 114) { i ++; continue; }
error = true; } while (0);
groups[1].end = i;
if (s[i] == 121) { i ++; } else { error = true; break; }
if (s[i] == 32) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strcat(output, "ist");
return j; } }

        // (\w[bcdfghjklmnpqrstvwxz])y ([abcdefghjklnopqrstuxz])
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
do {
if (48 <= s[i] && s[i] <= 57) { i ++; continue; }
if (65 <= s[i] && s[i] <= 90) { i ++; continue; }
if (s[i] == 95) { i ++; continue; }
if (97 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
do {
if (98 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 110) { i ++; continue; }
if (112 <= s[i] && s[i] <= 116) { i ++; continue; }
if (118 <= s[i] && s[i] <= 120) { i ++; continue; }
if (s[i] == 122) { i ++; continue; }
error = true; } while (0);
groups[1].end = i;
if (s[i] == 121) { i ++; } else { error = true; break; }
if (s[i] == 32) { i ++; } else { error = true; break; }
groups[2].start = i;
do {
if (97 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 108) { i ++; continue; }
if (110 <= s[i] && s[i] <= 117) { i ++; continue; }
if (s[i] == 120) { i ++; continue; }
if (s[i] == 122) { i ++; continue; }
error = true; } while (0);
groups[2].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strcat(output, "i");
strncat(output, s, groups[2].end - groups[2].start);
return j; } }

        // (\w[^aeiou]it) er(s?)
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
do {
if (48 <= s[i] && s[i] <= 57) { i ++; continue; }
if (65 <= s[i] && s[i] <= 90) { i ++; continue; }
if (s[i] == 95) { i ++; continue; }
if (97 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
do {
if (0 <= s[i] && s[i] <= 96) { i ++; continue; }
if (98 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 110) { i ++; continue; }
if (112 <= s[i] && s[i] <= 116) { i ++; continue; }
if (118 <= s[i] && s[i] <= 255) { i ++; continue; }
error = true; } while (0);
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
groups[1].end = i;
if (s[i] == 32) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 114) { i ++; } else { error = true; break; }
groups[2].start = i;
{
if (s[i] == 115) { i ++; } else { error = true; break; }
error = false; }
groups[2].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strcat(output, "or");
strncat(output, s, groups[2].end - groups[2].start);
return j; } }

        // (\wct) er(s?)
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
do {
if (48 <= s[i] && s[i] <= 57) { i ++; continue; }
if (65 <= s[i] && s[i] <= 90) { i ++; continue; }
if (s[i] == 95) { i ++; continue; }
if (97 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
if (s[i] == 99) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
groups[1].end = i;
if (s[i] == 32) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 114) { i ++; } else { error = true; break; }
groups[2].start = i;
{
if (s[i] == 115) { i ++; } else { error = true; break; }
error = false; }
groups[2].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strcat(output, "or");
strncat(output, s, groups[2].end - groups[2].start);
return j; } }

        // (\w[^aeiou]is)e er(s?)
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
do {
if (48 <= s[i] && s[i] <= 57) { i ++; continue; }
if (65 <= s[i] && s[i] <= 90) { i ++; continue; }
if (s[i] == 95) { i ++; continue; }
if (97 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
do {
if (0 <= s[i] && s[i] <= 96) { i ++; continue; }
if (98 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 110) { i ++; continue; }
if (112 <= s[i] && s[i] <= 116) { i ++; continue; }
if (118 <= s[i] && s[i] <= 255) { i ++; continue; }
error = true; } while (0);
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
groups[1].end = i;
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 32) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 114) { i ++; } else { error = true; break; }
groups[2].start = i;
{
if (s[i] == 115) { i ++; } else { error = true; break; }
error = false; }
groups[2].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strcat(output, "or");
strncat(output, s, groups[2].end - groups[2].start);
return j; } }

        // (\w[aeiou][bcdfghjklmnprstvwxyz]+at)e er(s?)
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
do {
if (48 <= s[i] && s[i] <= 57) { i ++; continue; }
if (65 <= s[i] && s[i] <= 90) { i ++; continue; }
if (s[i] == 95) { i ++; continue; }
if (97 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
do {
if (s[i] == 97) { i ++; continue; }
if (s[i] == 101) { i ++; continue; }
if (s[i] == 105) { i ++; continue; }
if (s[i] == 111) { i ++; continue; }
if (s[i] == 117) { i ++; continue; }
error = true; } while (0);
do {
if (98 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 110) { i ++; continue; }
if (s[i] == 112) { i ++; continue; }
if (114 <= s[i] && s[i] <= 116) { i ++; continue; }
if (118 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
while (true) {
do {
if (98 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 110) { i ++; continue; }
if (s[i] == 112) { i ++; continue; }
if (114 <= s[i] && s[i] <= 116) { i ++; continue; }
if (118 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
error = false; }
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
groups[1].end = i;
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 32) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 114) { i ++; } else { error = true; break; }
groups[2].start = i;
{
if (s[i] == 115) { i ++; } else { error = true; break; }
error = false; }
groups[2].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strcat(output, "or");
strncat(output, s, groups[2].end - groups[2].start);
return j; } }

        // (\w[bcdfghjklmnpqrstuvwxz])e ([aeoy])
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
do {
if (48 <= s[i] && s[i] <= 57) { i ++; continue; }
if (65 <= s[i] && s[i] <= 90) { i ++; continue; }
if (s[i] == 95) { i ++; continue; }
if (97 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
do {
if (98 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 110) { i ++; continue; }
if (112 <= s[i] && s[i] <= 120) { i ++; continue; }
if (s[i] == 122) { i ++; continue; }
error = true; } while (0);
groups[1].end = i;
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 32) { i ++; } else { error = true; break; }
groups[2].start = i;
do {
if (s[i] == 97) { i ++; continue; }
if (s[i] == 101) { i ++; continue; }
if (s[i] == 111) { i ++; continue; }
if (s[i] == 121) { i ++; continue; }
error = true; } while (0);
groups[2].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strncat(output, s, groups[2].end - groups[2].start);
return j; } }

        // (\w[bcdfghjklmnpqrstuvwxz])e ([aeiouy]\w)
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
do {
if (48 <= s[i] && s[i] <= 57) { i ++; continue; }
if (65 <= s[i] && s[i] <= 90) { i ++; continue; }
if (s[i] == 95) { i ++; continue; }
if (97 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
do {
if (98 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 110) { i ++; continue; }
if (112 <= s[i] && s[i] <= 120) { i ++; continue; }
if (s[i] == 122) { i ++; continue; }
error = true; } while (0);
groups[1].end = i;
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 32) { i ++; } else { error = true; break; }
groups[2].start = i;
do {
if (s[i] == 97) { i ++; continue; }
if (s[i] == 101) { i ++; continue; }
if (s[i] == 105) { i ++; continue; }
if (s[i] == 111) { i ++; continue; }
if (s[i] == 117) { i ++; continue; }
if (s[i] == 121) { i ++; continue; }
error = true; } while (0);
do {
if (48 <= s[i] && s[i] <= 57) { i ++; continue; }
if (65 <= s[i] && s[i] <= 90) { i ++; continue; }
if (s[i] == 95) { i ++; continue; }
if (97 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
groups[2].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strncat(output, s, groups[2].end - groups[2].start);
return j; } }

        // ([aoieuy](?:pod|log)) ([aoieuy])
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
do {
if (s[i] == 97) { i ++; continue; }
if (s[i] == 101) { i ++; continue; }
if (s[i] == 105) { i ++; continue; }
if (s[i] == 111) { i ++; continue; }
if (s[i] == 117) { i ++; continue; }
if (s[i] == 121) { i ++; continue; }
error = true; } while (0);
error = false; i_save = i; do {
if (s[i] == 112) { i ++; } else { error = true; break; }
if (s[i] == 111) { i ++; } else { error = true; break; }
if (s[i] == 100) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 108) { i ++; } else { error = true; break; }
if (s[i] == 111) { i ++; } else { error = true; break; }
if (s[i] == 103) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
groups[1].end = i;
if (s[i] == 32) { i ++; } else { error = true; break; }
groups[2].start = i;
do {
if (s[i] == 97) { i ++; continue; }
if (s[i] == 101) { i ++; continue; }
if (s[i] == 105) { i ++; continue; }
if (s[i] == 111) { i ++; continue; }
if (s[i] == 117) { i ++; continue; }
if (s[i] == 121) { i ++; continue; }
error = true; } while (0);
groups[2].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strncat(output, s, groups[2].end - groups[2].start);
return j; } }

        // ((?:[bcdfghjklmnprstvwxyz]|qu)[ae]l) y
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
error = false; i_save = i; do {
do {
if (98 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 110) { i ++; continue; }
if (s[i] == 112) { i ++; continue; }
if (114 <= s[i] && s[i] <= 116) { i ++; continue; }
if (118 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 113) { i ++; } else { error = true; break; }
if (s[i] == 117) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
do {
if (s[i] == 97) { i ++; continue; }
if (s[i] == 101) { i ++; continue; }
error = true; } while (0);
if (s[i] == 108) { i ++; } else { error = true; break; }
groups[1].end = i;
if (s[i] == 32) { i ++; } else { error = true; break; }
if (s[i] == 121) { i ++; } else { error = true; break; }
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strcat(output, "ly");
return j; } }

        // ((?:^|\W)(?:[bcdfghjklmnprstvwxyz]+|[bcdfghjklmnprstvwxyz]*qu)[aeiou])([bdfgklmnprstz]) (ed|en|er|ier|est|ing|y|ie|ies|iest|iness|ish|abl[ey]|ability|abilities)
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
error = false; i_save = i; do {
if (i != 0) { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
do {
if (0 <= s[i] && s[i] <= 47) { i ++; continue; }
if (58 <= s[i] && s[i] <= 64) { i ++; continue; }
if (91 <= s[i] && s[i] <= 94) { i ++; continue; }
if (s[i] == 96) { i ++; continue; }
if (123 <= s[i] && s[i] <= 255) { i ++; continue; }
error = true; } while (0);
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
do {
if (98 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 110) { i ++; continue; }
if (s[i] == 112) { i ++; continue; }
if (114 <= s[i] && s[i] <= 116) { i ++; continue; }
if (118 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
while (true) {
do {
if (98 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 110) { i ++; continue; }
if (s[i] == 112) { i ++; continue; }
if (114 <= s[i] && s[i] <= 116) { i ++; continue; }
if (118 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
error = false; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
while (true) {
do {
if (98 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 110) { i ++; continue; }
if (s[i] == 112) { i ++; continue; }
if (114 <= s[i] && s[i] <= 116) { i ++; continue; }
if (118 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
error = false; }
if (s[i] == 113) { i ++; } else { error = true; break; }
if (s[i] == 117) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
do {
if (s[i] == 97) { i ++; continue; }
if (s[i] == 101) { i ++; continue; }
if (s[i] == 105) { i ++; continue; }
if (s[i] == 111) { i ++; continue; }
if (s[i] == 117) { i ++; continue; }
error = true; } while (0);
groups[1].end = i;
groups[2].start = i;
do {
if (s[i] == 98) { i ++; continue; }
if (s[i] == 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 103) { i ++; continue; }
if (107 <= s[i] && s[i] <= 110) { i ++; continue; }
if (s[i] == 112) { i ++; continue; }
if (114 <= s[i] && s[i] <= 116) { i ++; continue; }
if (s[i] == 122) { i ++; continue; }
error = true; } while (0);
groups[2].end = i;
if (s[i] == 32) { i ++; } else { error = true; break; }
groups[3].start = i;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 100) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 114) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 114) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
if (s[i] == 103) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 121) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 104) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
do {
if (s[i] == 101) { i ++; continue; }
if (s[i] == 121) { i ++; continue; }
error = true; } while (0);
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
if (s[i] == 121) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
groups[3].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strncat(output, s, groups[2].end - groups[2].start);
strncat(output, s, groups[2].end - groups[2].start);
strncat(output, s, groups[3].end - groups[3].start);
return j; } }

        // ((?:[bcdfghjklmnprstvwxyz]|qu)a)([gbmptv]) (ed|en|er|ier|est|ing|y|ie|ies|iest|iness|ish|abl[ey]|ability|abilities)
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
error = false; i_save = i; do {
do {
if (98 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 110) { i ++; continue; }
if (s[i] == 112) { i ++; continue; }
if (114 <= s[i] && s[i] <= 116) { i ++; continue; }
if (118 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 113) { i ++; } else { error = true; break; }
if (s[i] == 117) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
if (s[i] == 97) { i ++; } else { error = true; break; }
groups[1].end = i;
groups[2].start = i;
do {
if (s[i] == 98) { i ++; continue; }
if (s[i] == 103) { i ++; continue; }
if (s[i] == 109) { i ++; continue; }
if (s[i] == 112) { i ++; continue; }
if (s[i] == 116) { i ++; continue; }
if (s[i] == 118) { i ++; continue; }
error = true; } while (0);
groups[2].end = i;
if (s[i] == 32) { i ++; } else { error = true; break; }
groups[3].start = i;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 100) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 114) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 114) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
if (s[i] == 103) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 121) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 104) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
do {
if (s[i] == 101) { i ++; continue; }
if (s[i] == 121) { i ++; continue; }
error = true; } while (0);
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
if (s[i] == 121) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
groups[3].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strncat(output, s, groups[2].end - groups[2].start);
strncat(output, s, groups[2].end - groups[2].start);
strncat(output, s, groups[3].end - groups[3].start);
return j; } }

        // ((?:[bcdfghjklmnprstvwxyz]|qu)e)([gbpv]) (ed|en|er|ier|est|ing|y|ie|ies|iest|iness|ish|abl[ey]|ability|abilities)
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
error = false; i_save = i; do {
do {
if (98 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 110) { i ++; continue; }
if (s[i] == 112) { i ++; continue; }
if (114 <= s[i] && s[i] <= 116) { i ++; continue; }
if (118 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 113) { i ++; } else { error = true; break; }
if (s[i] == 117) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
if (s[i] == 101) { i ++; } else { error = true; break; }
groups[1].end = i;
groups[2].start = i;
do {
if (s[i] == 98) { i ++; continue; }
if (s[i] == 103) { i ++; continue; }
if (s[i] == 112) { i ++; continue; }
if (s[i] == 118) { i ++; continue; }
error = true; } while (0);
groups[2].end = i;
if (s[i] == 32) { i ++; } else { error = true; break; }
groups[3].start = i;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 100) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 114) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 114) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
if (s[i] == 103) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 121) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 104) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
do {
if (s[i] == 101) { i ++; continue; }
if (s[i] == 121) { i ++; continue; }
error = true; } while (0);
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
if (s[i] == 121) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
groups[3].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strncat(output, s, groups[2].end - groups[2].start);
strncat(output, s, groups[2].end - groups[2].start);
strncat(output, s, groups[3].end - groups[3].start);
return j; } }

        // ((?:[bcdfghjklmnprstvwxyz]|qu)i)([gbmpv]) (ed|en|er|ier|est|ing|y|ie|ies|iest|iness|ish|abl[ey]|ability|abilities)
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
error = false; i_save = i; do {
do {
if (98 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 110) { i ++; continue; }
if (s[i] == 112) { i ++; continue; }
if (114 <= s[i] && s[i] <= 116) { i ++; continue; }
if (118 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 113) { i ++; } else { error = true; break; }
if (s[i] == 117) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
if (s[i] == 105) { i ++; } else { error = true; break; }
groups[1].end = i;
groups[2].start = i;
do {
if (s[i] == 98) { i ++; continue; }
if (s[i] == 103) { i ++; continue; }
if (s[i] == 109) { i ++; continue; }
if (s[i] == 112) { i ++; continue; }
if (s[i] == 118) { i ++; continue; }
error = true; } while (0);
groups[2].end = i;
if (s[i] == 32) { i ++; } else { error = true; break; }
groups[3].start = i;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 100) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 114) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 114) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
if (s[i] == 103) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 121) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 104) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
do {
if (s[i] == 101) { i ++; continue; }
if (s[i] == 121) { i ++; continue; }
error = true; } while (0);
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
if (s[i] == 121) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
groups[3].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strncat(output, s, groups[2].end - groups[2].start);
strncat(output, s, groups[2].end - groups[2].start);
strncat(output, s, groups[3].end - groups[3].start);
return j; } }

        // ((?:[bcdfghjklmnprstvwxyz]|qu)o)([gbdlv]) (ed|en|er|ier|est|ing|y|ie|ies|iest|iness|ish|abl[ey]|ability|abilities)
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
error = false; i_save = i; do {
do {
if (98 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 110) { i ++; continue; }
if (s[i] == 112) { i ++; continue; }
if (114 <= s[i] && s[i] <= 116) { i ++; continue; }
if (118 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 113) { i ++; } else { error = true; break; }
if (s[i] == 117) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
if (s[i] == 111) { i ++; } else { error = true; break; }
groups[1].end = i;
groups[2].start = i;
do {
if (s[i] == 98) { i ++; continue; }
if (s[i] == 100) { i ++; continue; }
if (s[i] == 103) { i ++; continue; }
if (s[i] == 108) { i ++; continue; }
if (s[i] == 118) { i ++; continue; }
error = true; } while (0);
groups[2].end = i;
if (s[i] == 32) { i ++; } else { error = true; break; }
groups[3].start = i;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 100) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 114) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 114) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
if (s[i] == 103) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 121) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 104) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
do {
if (s[i] == 101) { i ++; continue; }
if (s[i] == 121) { i ++; continue; }
error = true; } while (0);
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
if (s[i] == 121) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
groups[3].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strncat(output, s, groups[2].end - groups[2].start);
strncat(output, s, groups[2].end - groups[2].start);
strncat(output, s, groups[3].end - groups[3].start);
return j; } }

        // ([bcdfghjklmnprstvwxyz]u)([gbdlmntv]) (ed|en|er|ier|est|ing|y|ie|ies|iest|iness|ish|abl[ey]|ability|abilities)
        error = false;
        for (uint8_t j = 0; j < inp_len; j ++) {
            char *s = &inp[j]; uint8_t i = 0, i_save = 0, s_len = inp_len - j;
while (true) {
groups[1].start = i;
do {
if (98 <= s[i] && s[i] <= 100) { i ++; continue; }
if (102 <= s[i] && s[i] <= 104) { i ++; continue; }
if (106 <= s[i] && s[i] <= 110) { i ++; continue; }
if (s[i] == 112) { i ++; continue; }
if (114 <= s[i] && s[i] <= 116) { i ++; continue; }
if (118 <= s[i] && s[i] <= 122) { i ++; continue; }
error = true; } while (0);
if (s[i] == 117) { i ++; } else { error = true; break; }
groups[1].end = i;
groups[2].start = i;
do {
if (s[i] == 98) { i ++; continue; }
if (s[i] == 100) { i ++; continue; }
if (s[i] == 103) { i ++; continue; }
if (108 <= s[i] && s[i] <= 110) { i ++; continue; }
if (s[i] == 116) { i ++; continue; }
if (s[i] == 118) { i ++; continue; }
error = true; } while (0);
groups[2].end = i;
if (s[i] == 32) { i ++; } else { error = true; break; }
groups[3].start = i;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 100) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 114) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 114) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
if (s[i] == 103) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 121) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 110) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
if (s[i] == 104) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
do {
if (s[i] == 101) { i ++; continue; }
if (s[i] == 121) { i ++; continue; }
error = true; } while (0);
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
if (s[i] == 121) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
error = false; i_save = i; do {
if (s[i] == 97) { i ++; } else { error = true; break; }
if (s[i] == 98) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 108) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 116) { i ++; } else { error = true; break; }
if (s[i] == 105) { i ++; } else { error = true; break; }
if (s[i] == 101) { i ++; } else { error = true; break; }
if (s[i] == 115) { i ++; } else { error = true; break; }
} while (0); if (!error) { break; } i = i_save;
groups[3].end = i;
}
        if (!error) {
strncat(output, s, groups[1].end - groups[1].start);
strncat(output, s, groups[2].end - groups[2].start);
strncat(output, s, groups[2].end - groups[2].start);
strncat(output, s, groups[3].end - groups[3].start);
return j; } }
return -1; }
