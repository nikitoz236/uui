#include "val_text.h"
#include "str_val.h"

// #define dbg_printf(...) printf(__VA_ARGS__)
#define dbg_printf(...)

void val_text_ptr_to_str(char * str, const void * ptr, val_rep_t rep, val_text_t vt, unsigned len)
{
    unsigned str_end_idx = len;
    unsigned neg = 0;
    unsigned val_for_str = val_unpack(ptr, rep, &neg);
    if (neg) {
        str[0] = '-';
        str++;
        len--;
    }

    if (vt.p == 0) {
        dec_to_str_right_aligned(val_for_str, str, len, vt.zl);
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

                >01234567890        l 10, p 3, f 0
                >      3456
                >  3456             offset +4
                >  3456.0           незнаю что делать. просто пробел поменял на 0 после точки

            тут конечно магия и подгон под тесты, осощнание на поверхности.
        */

        int offset = vt.p - vt.f + 1;         // положительный сдвиг влево, отрицательный вправо
        dec_to_str_right_aligned(val_for_str, str, len - offset, vt.zl);
        dbg_printf("\nval text point %d, f %d, len %d offset %d, result str before add point >%s<\n", vt.p, vt.f, len, offset, str);


        unsigned zr = vt.zr;
        for (unsigned i = 0; i < vt.p; i++) {
            unsigned idx = len - i - 2;         // почему 2 ? одна из них это точка, вторая непонятно, это то куда сдвинется знак
            char c = str[idx];
            dbg_printf("  -- move char %c from %d to %d\n", c, idx, idx + 1);

            // бежим по всем знакоместам с конца строки до знакоместа точки и двигаем их справо на 1 символ
            // при этом если offset > 1, то там был мусора и мы сначала заполняем все нулями

            if(offset > 1) {
                c = '0';
                offset--;
                dbg_printf("     garbage in buffer replaced by 0\n", i);
            }

            // дальше думаем, нужны ли нам нули справа
            if (i == vt.p - 1) {
                // один разряд после точки точно должен быть 0
                zr = 1;
            }

            if (c == '0') {
                if (zr == 0) {
                    c = ' ';
                    dbg_printf("     char %d changed to [%c] was [0], zr %d\n", idx, c, zr);
                }
            } else if (c == ' ') {
                // разряды вообще могли кончится, так что тут будут пробелы заполненые на этапе dec_to_str_right_aligned
                if (zr) {
                    c = '0';
                    dbg_printf("     char %d changed to [%c] was [ ], zr %d\n", idx, c, zr);
                }
            } else {
                // встретили значачащий разряд, левее до точки будем заполнять нулями
                zr = 1;
            }

            str[idx + 1] = c;
        }
        str[len - vt.p - 1] = '.';

        // test 28 - если у нас не осталось значаших разрядов слева от нуля, то добавим туда 0
        if (str[len - vt.p - 2] == ' ') {
            str[len - vt.p - 2] = '0';
        }

        str[str_end_idx] = 0;
    }
}
