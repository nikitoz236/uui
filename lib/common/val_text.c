#include "val_text.h"
#include "str_val.h"

void val_text_to_str(char * str, const void * val, const val_text_t * tv)
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

