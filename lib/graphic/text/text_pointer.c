
#include "text_pointer.h"

unsigned text_ptr_next_str(tptr_t * tptr)
{
    if (tptr->tlim.y) {
        if (tptr->cpos.y >= (tptr->tlim.y - 1)) {
            return 0;
        }
    }

    tptr->cpos.x = 0;
    tptr->cpos.y++;

    tptr->cxy.x =tptr->fxy.x;
    tptr->cxy.y += tptr->cstep.y;

    return 1;
}

unsigned text_ptr_next_char(tptr_t * tptr)
{
    if (tptr->tlim.x) {
        if (tptr->cpos.x >= (tptr->tlim.x - 1)) {
            return text_ptr_next_str(tptr);
        }
    }

    tptr->cpos.x++;
    tptr->cxy.x += tptr->cstep.x;

    return 1;
}

unsigned text_ptr_prev_char(tptr_t * tptr)
{
    if (tptr->cpos.x) {
        tptr->cpos.x--;
        tptr->cxy.x -= tptr->cstep.x;
        return 1;
    }
    // а хуй его знает как не предыдущую строку перепрыгнуть, там же не до конца символы, надо знать скок символов в строке. пизда
    return 0;
}

unsigned text_ptr_remain_str(tptr_t * tptr)
{
    return tptr->tlim.x - tptr->cpos.x;
}

tptr_t text_ptr_create(const lcd_font_cfg_t * fcfg, xy_t pos_px, xy_t limit_chars)
{
    tptr_t t;

    t.fcfg = fcfg;

    if (fcfg) {
        unsigned scale = fcfg_scale(fcfg);
        for (unsigned d = 0; d < DIMENSION_COUNT; d++) {
            unsigned gap = fcfg_gap(fcfg, d);
            t.cstep.ca[d] = (fcfg->font->size.ca[d] * scale) + gap;
        }
    } else {
        t.cstep = (xy_t){};
    }

    t.tlim = limit_chars;
    // if (limit_chars) {
    //     t.tlim = *limit_chars;
    // } else {
    //     t.tlim = (xy_t){};
    // }

    // не понимаю зачем раньше передавал положение черех указатель? чтобы сослаться на поле какойто формы?
    t.fxy = pos_px;
    t.cxy = pos_px;
    return t;
}

unsigned text_ptr_set_char_pos(tptr_t * tptr, xy_t pos)
{
    for (unsigned d = 0; d < DIMENSION_COUNT; d++) {
        int shift = pos.ca[d];

        if (shift < 0) {
            if (tptr->tlim.ca[d]) {
                shift += tptr->tlim.ca[d]; //  - ( -1 * shift )
            } else {
                shift = 0;
            }
        } else {
            if (shift >= tptr->tlim.ca[d]) {
                return 0;
            }
        }
        tptr->cpos.ca[d] = shift;
        tptr->cxy.ca[d] = tptr->fxy.ca[d] + (shift * tptr->cstep.ca[d]);
    }
    return 1;
}

tptr_t text_ptr_export(tptr_t * tp, xy_t size_chars)
{
    tptr_t field = { .cstep = tp->cstep, .cxy = tp->cxy, .fcfg = tp->fcfg, .fxy = tp->cxy };

    for (unsigned d = 0; d < DIMENSION_COUNT; d++) {
        field.tlim.ca[d] = tp->tlim.ca[d] - tp->cpos.ca[d];
        if (size_chars.ca[d]) {
            if (field.tlim.ca[d] > size_chars.ca[d]) {
                field.tlim.ca[d] = size_chars.ca[d];
            }
        }
    }

    return field;
}
