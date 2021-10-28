#pragma once

#include <stdint.h>

#define WORD_ENDING_SIZE 8

int8_t regex_ortho(const char *const word, const char *const suffix, char *const output);
int8_t process_ortho(const char *word, const char *suffix, char *output);
