#pragma once

#include <stdint.h>

#define WORD_ENDING_SIZE 8

int8_t process_ortho(const char *word, const char *suffix, char *output, uint8_t const *clause);
