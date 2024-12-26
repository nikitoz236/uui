#include "forms.h"
#include "forms_edge.h"

// уменьшает форму по одному измерению d на offset пикселей со стороны cut_edge
// в аргументе указываем край от которого отрезаем, и сколько отрезаем
// оставшаяся часть остается на месте
void form_cut(form_t * f, dimension_t d, form_edge_t cut_edge, unsigned offset);

// уменьшает форму по одному измерению d, оставля указаный край edge, с новым размером new_size
// тоже что и from_cut, только указываем что и сколько оставить
// оставшаяся часть остается на месте
void form_reduce(form_t * f, dimension_t d, form_edge_t edge, unsigned new_size);

static inline form_t form_split_size(form_t * f, dimension_t d, form_edge_t edge, unsigned offset)
{
    form_t res = *f;
    form_reduce(&res, d, edge, offset);
    return res;
}

static inline form_t form_split_half(form_t * f, dimension_t d, form_edge_t edge)
{
    return form_split_size(f, d, edge, f->s.ca[d] / 2);
}
