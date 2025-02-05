#include "fonts.h"

const font_t font_3x5 = {
    .size = {
        .w = 3,
        .h = 5,
    },
    .start = '-',
    .end = ':',
    .n_replaces = 6,
    .font = {
        // replaces
        'A',  0x1e, 0x05, 0x1f,     // 41 A
        'B',  0x1f, 0x15, 0x0a,     // 42 B
        'C',  0x0e, 0x11, 0x11,     // 43 C
        'D',  0x1f, 0x11, 0x0e,     // 44 D
        'E',  0x1f, 0x15, 0x15,     // 45 E
        'F',  0x1f, 0x05, 0x05,     // 46 F

        // solid bitmaps
        0x04, 0x04, 0x04,           // 2d -
        0x00, 0x10, 0x00,           // 2e .
        0x00, 0x00, 0x00,           // 2f /
        0x1F, 0x11, 0x1F,           // 30 0
        0x12, 0x1F, 0x10,           // 31 1
        0x1D, 0x15, 0x17,           // 32 2
        0x15, 0x15, 0x1F,           // 33 3
        0x07, 0x04, 0x1F,           // 34 4
        0x17, 0x15, 0x1D,           // 35 5
        0x1F, 0x15, 0x1D,           // 36 6
        0x01, 0x01, 0x1F,           // 37 7
        0x1F, 0x15, 0x1F,           // 38 8
        0x17, 0x15, 0x1F,           // 39 9
        0x00, 0x0A, 0x00,           // 3a :
    }
};
