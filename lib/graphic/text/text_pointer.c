
#include "text_pointer.h"

unsigned text_ptr_next_str(tptr_t * tptr)
{
    if (tptr->tf.lim.y) {
        if (tptr->cpos.y >= (tptr->tf.lim.y - 1)) {
            return 0;
        }
    }

    tptr->cpos.x = 0;
    tptr->cpos.y++;

    tptr->cxy.x =tptr->tf.pos.x;
    tptr->cxy.y += tptr->cstep.y;

    return 1;
}

unsigned text_ptr_next_char(tptr_t * tptr)
{
    if (tptr->tf.lim.x) {
        if (tptr->cpos.x >= (tptr->tf.lim.x - 1)) {
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
    return tptr->tf.lim.x - tptr->cpos.x;
}

tptr_t text_ptr_create(tf_t tf)
{
    tptr_t t = { .tf = tf, .cxy = tf.pos };
    unsigned scale = fcfg_scale(tf.fcfg);
    for (unsigned d = 0; d < DIMENSION_COUNT; d++) {
        unsigned gap = fcfg_gap(tf.fcfg, d);
        t.cstep.ca[d] = (tf.fcfg->font->size.ca[d] * scale) + gap;
    }
    return t;
}

unsigned text_ptr_set_char_pos(tptr_t * tptr, xy_t pos)
{
    for (unsigned d = 0; d < DIMENSION_COUNT; d++) {
        int shift = pos.ca[d];

        if (shift < 0) {
            if (tptr->tf.lim.ca[d]) {
                shift += tptr->tf.lim.ca[d]; //  - ( -1 * shift )
            } else {
                shift = 0;
            }
        } else {
            if (shift >= tptr->tf.lim.ca[d]) {
                return 0;
            }
        }
        tptr->cpos.ca[d] = shift;
        tptr->cxy.ca[d] = tptr->tf.pos.ca[d] + (shift * tptr->cstep.ca[d]);
    }
    return 1;
}

tptr_t text_ptr_export(tptr_t * tp, xy_t size_chars)
{
    tptr_t field = { .cstep = tp->cstep, .cxy = tp->cxy, .tf.fcfg = tp->tf.fcfg, .tf.pos = tp->cxy };

    for (unsigned d = 0; d < DIMENSION_COUNT; d++) {
        field.tf.lim.ca[d] = tp->tf.lim.ca[d] - tp->cpos.ca[d];
        if (size_chars.ca[d]) {
            if (field.tf.lim.ca[d] > size_chars.ca[d]) {
                field.tf.lim.ca[d] = size_chars.ca[d];
            }
        }
    }

    return field;
}
