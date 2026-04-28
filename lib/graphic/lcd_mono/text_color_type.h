#pragma once

#include <stdint.h>

typedef struct __attribute__((packed)) {
    uint8_t inverted : 1;
} text_color_t;
