#include "fonts.h"

font_t font_5x5 = {
    .w = 5,
    .h = 5,
    .start = '-',
    .end = 'Z',
    .font = {
        0x04, 0x04, 0x04, 0x04, 0x04,		// 2d -
        0x00, 0x00, 0x18, 0x18, 0x00,		// 2e .
        0x10, 0x08, 0x04, 0x02, 0x01,		// 2f /
        0x1f, 0x11, 0x11, 0x11, 0x1f,		// 30 0
        0x12, 0x12, 0x1f, 0x10, 0x10,		// 31 1
        0x1d, 0x15, 0x15, 0x15, 0x17,		// 32 2
        0x15, 0x15, 0x15, 0x15, 0x1f,		// 33 3
        0x0c, 0x0a, 0x09, 0x1f, 0x08,		// 34 4
        0x17, 0x15, 0x15, 0x15, 0x1d,		// 35 5
        0x1f, 0x15, 0x15, 0x15, 0x1d,		// 36 6
        0x01, 0x11, 0x09, 0x05, 0x03,		// 37 7
        0x1f, 0x15, 0x15, 0x15, 0x1f,		// 38 8
        0x17, 0x15, 0x15, 0x15, 0x1f,		// 39 9

        0x00, 0x00, 0x1b, 0x1b, 0x00,		// 3a :

        0x55, 0xAA, 0x55, 0xAA, 0x55,		// 3b ;
        0x55, 0xAA, 0x55, 0xAA, 0x55,		// 3c <
        0x55, 0xAA, 0x55, 0xAA, 0x55,		// 3d =
        0x55, 0xAA, 0x55, 0xAA, 0x55,		// 3e >
        0x55, 0xAA, 0x55, 0xAA, 0x55,		// 3f ?
        0x55, 0xAA, 0x55, 0xAA, 0x55,		// 40 @

        0x1e, 0x09, 0x09, 0x09, 0x1e,		// 41 A
        0x1f, 0x15, 0x15, 0x15, 0x0e,		// 42 B
        0x0e, 0x11, 0x11, 0x11, 0x11,		// 43 C
        0x1f, 0x11, 0x11, 0x11, 0x0e,		// 44 D
        0x1f, 0x15, 0x15, 0x15, 0x15,		// 45 E
        0x1f, 0x05, 0x05, 0x05, 0x05,		// 46 F
        0x0e, 0x11, 0x15, 0x15, 0x1d,		// 47 G
        0x1f, 0x04, 0x04, 0x04, 0x1f,		// 48 H
        0x00, 0x11, 0x1f, 0x11, 0x00,		// 49 I
        0x10, 0x10, 0x11, 0x11, 0x0f,		// 4a J
        0x1f, 0x04, 0x04, 0x0a, 0x11,		// 4b K
        0x1f, 0x10, 0x10, 0x10, 0x10,		// 4c L
        0x1f, 0x02, 0x04, 0x02, 0x1f,		// 4d M
        0x1f, 0x02, 0x04, 0x08, 0x1f,		// 4e N
        0x0e, 0x11, 0x11, 0x11, 0x0e,		// 4f O
        0x1f, 0x09, 0x09, 0x09, 0x06,		// 50 P
        0x0e, 0x11, 0x11, 0x09, 0x16,		// 51 Q
        0x1f, 0x09, 0x09, 0x09, 0x16,		// 52 R
        0x12, 0x15, 0x15, 0x15, 0x09,		// 53 S
        0x01, 0x01, 0x1f, 0x01, 0x01,		// 54 T
        0x0f, 0x10, 0x10, 0x10, 0x0f,		// 55 U
        0x03, 0x0c, 0x10, 0x0c, 0x03,		// 56 V
        0x0f, 0x10, 0x0c, 0x10, 0x0f,		// 57 W
        0x11, 0x0a, 0x04, 0x0a, 0x11,		// 58 X
        0x11, 0x0a, 0x04, 0x02, 0x01,		// 59 Y
        0x11, 0x19, 0x15, 0x13, 0x11,		// 5a Z
    }
};
