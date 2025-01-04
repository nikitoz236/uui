#pragma once
#include <stdint.h>

typedef enum {
    VAL_SIZE_8,
    VAL_SIZE_16,
    VAL_SIZE_32
} val_size_t;

static inline unsigned val_ptr_to_usnigned(const void * ptr, val_size_t size)
{
    switch (size) {
        case VAL_SIZE_8: return *(uint8_t *)ptr;
        case VAL_SIZE_16: return *(uint16_t *)ptr;
        case VAL_SIZE_32: return *(uint32_t *)ptr;
    }
    return 0;
}

static inline int val_ptr_to_signed(const void * ptr, val_size_t size)
{
    switch (size) {
        case VAL_SIZE_8: return *(int8_t *)ptr;
        case VAL_SIZE_16: return *(int16_t *)ptr;
        case VAL_SIZE_32: return *(int32_t *)ptr;
    }
    return 0;
}
