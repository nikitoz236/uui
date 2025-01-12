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

        unsigned len = tv->l - n;
        if (tv->p == 0) {
            dec_to_str_right_aligned(val_for_str, str, len, tv->zl);
        } else {
            /*
                например у нас 10 знакомест и число 1234536

                    >01234567890
                    >   1234536

                    >01234567890
                    >  1234536          offset +1
                    >  1234.536         перекдадываем 10 -> 11, 9 -> 10, 8 -> 9 и ставим точку в 6 = 10 - 3 - 1

                    >01234567890
                    >    1234536        offset -1
                    >    1234.5         перекладываем 8 -> 9 и ставим точку в 8 = 10 - 1 - 1

                    >01234567890
                    >1234536            offset +3
                    >123453.6           перекладываем 6-> 7, ставим точку в 6, но еще нужно заполнить пробелами 8 и 9, 0 в 10

                тут конечно магия и подгон под тесты, осощнание на поверхности.
            */

            int offset = tv->p - tv->f + 1;         // положительный сдвиг влево, отрицательный вправо
            // printf("\nval text point %d, f %d, len %d offset %d\n", tv->p, tv->f, len, offset);
            dec_to_str_right_aligned(val_for_str, str, len - offset, tv->zl);
            unsigned zr = tv->zr;
            for (unsigned i = 0; i < tv->p; i++) {
                unsigned idx = len - i - 2;         // почему 2 ? одна из них это точка, вторая непонятно, это то куда сдвинется знак
                char c = str[idx];                  // начиная с \0 в конце строки
                // printf("  -- move char %c from %d to %d\n", c, idx, idx + 1);
                if(offset > 1) {
                    if (tv->zr) {
                        c = '0';
                    } else {
                        c = ' ';
                    }
                    offset--;
                    // printf("    char changed to [%c]\n", c);
                } else {
                    if (c == '0') {
                        if (zr == 0) {
                            c = ' ';
                            // printf("    char changed to [%c]\n", c);
                        }
                    } else {
                        zr = 1;
                    }
                }
                str[idx + 1] = c;
            }
            str[tv->l - tv->p - 1] = '.';
            str[tv->l] = 0;
        }
    }
}
