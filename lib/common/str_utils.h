#pragma once
#include <stdint.h>

unsigned str_cmp(const void * s1, const void * s2, unsigned len);
void str_cp(void * d, const void * s, unsigned len);
unsigned str_find(const char * str, unsigned max_len, char c);
void str_fill(void * d, unsigned len, uint8_t c);

static inline unsigned str_len(const char * str, unsigned max_len)
{
    return str_find(str, max_len, 0);
}
