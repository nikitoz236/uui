#pragma once
#include <stdint.h>
#include "val_pack.h"

typedef enum {
    X1      = 0,
    X10     = 1,
    X100    = 2,
    X1000   = 3,
} dec_factor_t;

typedef struct __attribute__((packed)) {
    dec_factor_t f : 2;         // множитель
    uint8_t zl : 1;             // дополнить нулями слева
    uint8_t zr : 1;             // дополнить нулями после запятой
    uint8_t p : 3;              // знаков после запятой
} val_text_t;

void val_text_ptr_to_str(char * str, const void * ptr, val_rep_t rep, val_text_t vt, unsigned len);
