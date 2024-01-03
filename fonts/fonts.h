#pragma once
#include <stdint.h>

typedef const struct {
    uint8_t h;
    uint8_t w;
    char start;
    char end;
    uint8_t n_replaces;
    uint8_t font[];
} font_t;

const uint8_t * font_char_ptr(char c, font_t * f, uint8_t * col_step);
