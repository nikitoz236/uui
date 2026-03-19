#pragma once
#include <stdint.h>

typedef uint16_t utf16_t;
typedef uint8_t utf8_t;

unsigned utf16_to_utf8(uint8_t * dst, unsigned dst_max, const uint16_t * src, unsigned src_len);
