#include "forms_split.h"

void form_cut(form_t * f, dimension_t d, form_edge_t cut_edge, unsigned offset)
{
    if (cut_edge == EDGE_U) {
        f->p.ca[d] += offset;
        f->s.ca[d] -= offset;
    } else {
        f->s.ca[d] = offset;
    }
}

void form_reduce(form_t * f, dimension_t d, form_edge_t edge, unsigned new_size)
{
    if (edge == EDGE_U) {
        f->s.ca[d] = new_size;
    } else {
        f->p.ca[d] += f->s.ca[d] - new_size;
        f->s.ca[d] = new_size;
    }
}

