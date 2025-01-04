#pragma once
#include <stdint.h>
#include "val_pack.h"

typedef enum {
    X_1,
    X_10,
    X_100,
    X_1000,
} dec_factor_t;

typedef struct __attribute__((packed)) {
    uint8_t l : 4;              // длина строки в символах
    val_size_t vs : 2;          // размер значения в памяти
    dec_factor_t f : 2;         // множитель
    uint8_t p : 3;              // знаков после запятой
    uint8_t s : 1;              // знак числа
    uint8_t zl : 1;             // дополнить нулями слева
    uint8_t zr : 1;             // дополнить нулями после запятой
    enum {
        DEC,
        HEX
    } t : 2;                    // тип печатаемого вывода
} text_val_t;

void text_val_to_str(char * str, const void * val, const text_val_t * tv);
