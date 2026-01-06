
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

void text_ptr_init(tptr_t * tptr, const lcd_font_cfg_t * fcfg, xy_t pos_px, const xy_t * limit_chars)
{
    if (!tptr) {
        return;
    }

    tptr->fcfg = fcfg;

    if (fcfg) {
        unsigned scale = fcfg_scale(fcfg);
        for (unsigned d = 0; d < DIMENSION_COUNT; d++) {
            unsigned gap = fcfg_gap(fcfg, d);
            tptr->cstep.ca[d] = (fcfg->font->size.ca[d] * scale) + gap;
        }
    } else {
        tptr->cstep = (xy_t){};
    }

    if (limit_chars) {
        tptr->tlim = *limit_chars;
    } else {
        tptr->tlim = (xy_t){};
    }

    // не понимаю зачем раньше передавал положение черех указатель? чтобы сослаться на поле какойто формы?
    tptr->fxy = pos_px;
    tptr->cxy = pos_px;
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
}
