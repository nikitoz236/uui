#pragma once
#include <stdint.h>

typedef uint16_t lcd_color_t;

#define __convertColor(r, g, b)	((((r) & 0xF8) << 8) + (((g) & 0xFC) << 3) + (((b) & 0xF8) >> 3))
#define __convertColorHEX(v)		(__convertColor((v >> 16), (v >> 8), (v)))

#define COLOR(x)    __convertColorHEX(x)
