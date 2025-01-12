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
    /*DEPRICATED*/  uint8_t l : 4;              // длина строки в символах
    /*DEPRICATED*/  val_size_t vs : 2;          // размер значения в памяти
    dec_factor_t f : 2;         // множитель
    uint8_t p : 3;              // знаков после запятой
    /*DEPRICATED*/  uint8_t s : 1;              // знак числа
    uint8_t zl : 1;             // дополнить нулями слева
    uint8_t zr : 1;             // дополнить нулями после запятой
    /*DEPRICATED*/  enum {
        DEC,
        HEX
    } t : 2;                    // тип печатаемого вывода
} val_text_t;

void val_text_to_str(char * str, const void * val, const val_text_t * tv);
