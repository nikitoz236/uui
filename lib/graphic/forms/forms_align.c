#include "forms_align.h"

xy_t align_form_pos(const form_t * pf, xy_t s, align_t a, xy_t padding)
{
    xy_t p = pf->p;
    for (unsigned d = 0; d < DIMENSION_COUNT; d++) {
        if (a.ca[d].center) {
            p.ca[d] += ((pf->s.ca[d] - s.ca[d]) / 2);
        } else {
            unsigned side = a.ca[d].side;
            if (a.ca[d].edge == EDGE_R) {
                side = !side;
                p.ca[d] += pf->s.ca[d];
            }
            if (side == ALIGN_I) {
                p.ca[d] += padding.ca[d];
            } else {
                p.ca[d] -= (padding.ca[d] + s.ca[d]);
            }
        }
    }
    return p;
}

xy_t size_add_padding(xy_t size, xy_t padding)
{
    return (xy_t){
        .x = size.x + padding.x * 2,
        .y = size.y + padding.y * 2
    };
}
