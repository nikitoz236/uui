#include "forms.h"

static inline void form_grid_by_coord(const form_t * p, form_t * f, xy_t size, xy_t margin, xy_t coord)
{
    for (unsigned d = 0; d < DIMENSION_COUNT; d++) {
        f->s.ca[d] = ((p->s.ca[d] + margin.ca[d]) / size.ca[d]) - margin.ca[d];
        f->p.ca[d] = p->p.ca[d] + ((f->s.ca[d] + margin.ca[d]) * coord.ca[d]);
    }
}

static inline unsigned form_grid_by_idx(const form_t * p, form_t * f, xy_t size, xy_t margin, unsigned idx)
{
    xy_t coord;
    coord.y = idx / size.x;
    if (coord.y >= size.y) {
        return 0;
    }
    coord.x = idx % size.x;
    form_grid_by_coord(p, f, size, margin, coord);
    return 1;
}
