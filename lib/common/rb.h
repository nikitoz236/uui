#pragma once
#include <stdint.h>

typedef struct {
    uint16_t head;
    uint16_t tail;
    uint8_t data[];
} rb_t;
