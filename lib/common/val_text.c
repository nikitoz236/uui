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

        /*
            надо написать сюда еще обработку знаков после запятой, и нолей после запятой
            p определяет место для точки. нужно както разрывать строку на две части, и вставлять точку


        */

        unsigned len = tv->l - n;


        if (tv->p) {
            int offset = tv->f - tv->p - 1; // знавковый, положительный сдвиг вправо

            printf("\nval text point %d, f %d, len %d offset %d\n", tv->p, tv->f, len, offset);

            dec_to_str_right_aligned(val_for_str, str, len + offset, tv->zl);

            for (unsigned i = 0; i < tv->p; i++) {
                unsigned idxs = len - i - 2;
/*
01234567890
   1234536

01234567890
  1234536           offset -1
  1234.536          перекдадываем 10 -> 11, 9 -> 10, 8 -> 9 и ставим точку в 6 = 10 - 3 - 1

01234567890
    1234536         offset +1
    1234.5          перекладываем 8 -> 9 и ставим точку в 8 = 10 - 1 - 1

*/
                char c = str[idxs]; // начиная с \0 в конце строки
                str[idxs + 1] = c;

                printf("  -- move char %c from %d to %d\n", c, idxs, idxs + 1);
            }
            str[tv->l - tv->p - 1] = '.';
            str[tv->l] = 0;
        } else {
            dec_to_str_right_aligned(val_for_str, str, len, tv->zl);
        }

    }
}

