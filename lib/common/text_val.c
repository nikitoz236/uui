#include "text_val.h"
#include "str_val.h"

unsigned val_ptr_to_usnigned(const void * ptr, val_size_t size)
{
    switch (size) {
        case VAL_SIZE_8: return *(uint8_t *)ptr;
        case VAL_SIZE_16: return *(uint16_t *)ptr;
        case VAL_SIZE_32: return *(uint32_t *)ptr;
    }
    return 0;
}

int val_ptr_to_signed(const void * ptr, val_size_t size)
{
    switch (size) {
        case VAL_SIZE_8: return *(int8_t *)ptr;
        case VAL_SIZE_16: return *(int16_t *)ptr;
        case VAL_SIZE_32: return *(int32_t *)ptr;
    }
    return 0;
}

void text_val_to_str(char * str, const void * val, const text_val_t * tv)
{
    unsigned n = 0;
    if (tv->t == DEC) {
        unsigned val_for_str;
        if (tv->s) {
            int val_signed = val_ptr_to_signed(val, tv->vs);
            if (val_signed < 0) {
                val_signed = -val_signed;
                n = 1;
            }
            val_for_str = val_signed;
        } else {
            val_for_str = val_ptr_to_usnigned(val, tv->vs);
        }

        if (n) {
            str[0] = '-';
            str++;
        }

        dec_to_str_right_aligned(val_for_str, str, tv->l - n, tv->zl);
    }
}

