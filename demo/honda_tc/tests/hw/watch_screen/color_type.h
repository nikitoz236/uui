#pragma once

#include <stdint.h>

typedef uint16_t lcd_color_t;

typedef union {
    struct __attribute__((packed)) {
        unsigned b : 8;     // LSB
        unsigned g : 8;
        unsigned r : 8;
    };
    unsigned val;
} __color_rgb888_t;

typedef union  {
    struct __attribute__((packed)) {
        unsigned b : 5;     // LSB
        unsigned g : 6;
        unsigned r : 5;
    };
    lcd_color_t val;
} __color_rgb_565_t;

// #define COLOR(x) ((__color_rgb_565_t){ \
//         .r = ((__color_rgb888_t){ .val = x }).r >> (8 - 5), \
//         .g = ((__color_rgb888_t){ .val = x }).g >> (8 - 6), \
//         .b = ((__color_rgb888_t){ .val = x }).b >> (8 - 5), \
//     }).val

// #define COLOR(x) (lcd_color_t)((__color_rgb_565_t){ \
//         .r = (x >> 16) >> (8 - 5), \
//         .g = (x >> 8) >> (8 - 6), \
//         .b = (x >> 0) >> (8 - 5), \
//     })

#define __convertColor(r, g, b)	((((r) & 0xF8) << 8) + (((g) & 0xFC) << 3) + (((b) & 0xF8) >> 3))
#define __convertColorHEX(v)		(__convertColor((v >> 16), (v >> 8), (v)))

#define COLOR(x)    __convertColorHEX(x)
