#pragma once
#include <stdint.h>

typedef struct {
    uint16_t head;          // данные кладутся по этому индексу
    uint16_t tail;          // догоняет head, данные забираются по этому индексу
    uint8_t data[];
} rb_t;

typedef struct __attribute__((packed)) {
    rb_t * rb;
    uint16_t size;
} rb_desc_t;

#define RB_CREATE(name, size) \
    struct { \
        rb_t rb; \
        uint8_t data[size]; \
    } name = {}

// вообще надо структуру сделать которая хранит размер и указатель на буфер

static inline unsigned rb_available(const rb_desc_t * rb)
{
    unsigned t = rb->rb->tail;
    unsigned h = rb->rb->head + 1;
    if (rb->rb->head >= rb->rb->tail) {
        t += rb->size;
    }
    return t - h;
}

static inline void rb_put(const rb_desc_t * rb, uint8_t data)
{
    rb->rb->data[rb->rb->head++] = data;
    if (rb->rb->head == rb->size) {
        rb->rb->head = 0;
    }
}

static inline uint8_t rb_get(const rb_desc_t * rb)
{
    uint8_t data = rb->rb->data[rb->rb->tail++];
    if (rb->rb->tail == rb->size) {
        rb->rb->tail = 0;
    }
    return data;
}

static inline unsigned rb_is_full(const rb_desc_t * rb)
{
    if (rb->rb->tail == 0) {
        if ((rb->rb->head == (rb->size - 1))) {
            return 1;
        }
    } else {
        if ((rb->rb->head == (rb->rb->tail - 1))) {
            return 1;
        }
    }
    return 0;
}

static inline unsigned rb_is_empty(const rb_desc_t * rb)
{
    if (rb->rb->head == rb->rb->tail) {
        return 1;
    }
    return 0;
}
