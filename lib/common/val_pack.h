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

static inline void val_ptr_from_unsigned(void * ptr, val_size_t size, unsigned val)
{
    switch (size) {
        case VAL_SIZE_8: *(uint8_t *)ptr = val; break;
        case VAL_SIZE_16: *(uint16_t *)ptr = val; break;
        case VAL_SIZE_32: *(uint32_t *)ptr = val; break;
    }
}

static inline void val_ptr_from_signed(void * ptr, val_size_t size, int val)
{
    switch (size) {
        case VAL_SIZE_8: *(int8_t *)ptr = val; break;
        case VAL_SIZE_16: *(int16_t *)ptr = val; break;
        case VAL_SIZE_32: *(int32_t *)ptr = val; break;
    }
}
