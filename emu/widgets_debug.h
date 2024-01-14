#pragma once
#include "widgets.h"
#include <stdio.h>

void widgets_print_ctx(unsigned level, ui_ctx_t * ctx, unsigned max_num);

static inline void form_print(char * name, form_t * f)
{
    printf("%s: p(%d, %d), s(%d, %d)\n", name, f->p.x, f->p.y, f->s.x, f->s.y);
}
