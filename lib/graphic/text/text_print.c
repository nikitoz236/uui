#include "text_print.h"
#include "print_char.h"

static unsigned text_ptr_process_char(tptr_t * tptr, char c)
{
    if (c == '\n') {
        text_ptr_next_str(tptr);
        // а что делать если закончилось место?
        return 1;
    }
    if (c == '\b') {
        text_ptr_prev_char(tptr);
        return 1;
    }
    return 0;
}

void text_print(tptr_t * tptr, const char * str, text_color_t color, unsigned len)
{
    while (1) {
        char c = 0;

        if (str) {
            c = *str;
            if (c) {
                str++;
            }
        }

        if (c == 0) {
            if (len == 0) {
                return;
            }
            // если строка закончилась или отсутствует, но указан len
            // заполняем оставшиеся от len знакоместа пробелами
            c = ' ';
        }

        if (text_ptr_process_char(tptr, c) == 0) {
            // а какже перевод строки ? а нужно ли чистить оставшуюся часть строки - не реализовано. в строке останется мусор
            // зазор между буквами нужен чтобы не выйти за правый край формы

            print_char(c, tptr->cxy, tptr->tf.fcfg->font, color, fcfg_scale(tptr->tf.fcfg));

            if (text_ptr_next_char(tptr) == 0) {
                return;
            }
        }

        if (len) {
            len--;
            if (len == 0) {
                return;
            }
        }
    }
}
