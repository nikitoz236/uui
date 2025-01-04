#include "fonts.h"
#include "misc.h"

/*
    возвращает указатель на первый байт первого столбика символа c в шрифте f
    в col_step кладет шаг в байтах между столбиками
    если вернет 0 то символ не был найден
*/

const uint8_t * font_char_ptr(char c, const font_t * f, unsigned * col_step)
{
    uint8_t hbytes = DIV_ROUND_UP(f->size.h, 8);
    *col_step = hbytes;
    uint8_t bytes_per_char = hbytes * f->size.w;
    if ((c >= f->start) && (c <= f->end)) {
        uint8_t cidx = c - f->start;
        const uint8_t * solid_bitmaps = &f->font[f->n_replaces * (bytes_per_char + 1)];
        return &solid_bitmaps[cidx * bytes_per_char];
    } else {
        const uint8_t * ptr = f->font;
        for (uint8_t i = 0; i < f->n_replaces; i++) {
            if (*ptr == c) {
                return ptr + 1;
            }
            ptr += bytes_per_char + 1;
        }
    }
    return 0;
}
