#pragma once
#include <stdint.h>

typedef struct {
    uint16_t head;
    uint16_t tail;
    uint8_t data[];
} rb_t;

#define RB_CREATE(name, size) \
    struct { \
        rb_t rb; \
        uint8_t data[size]; \
    } name = {}

// вообще надо структуру сделать которая хранит размер и указатель на буфер
