#pragma once
#include <stdint.h>
#include "gfx.h"
#include "forms.h"
#include "color_type.h"

static inline void emu_set_color(lcd_color_t color)
{
    struct color {
        int b : 8;
        int g : 8;
        int r : 8;
    };
    struct color * cp = (struct color *)&color;
    gfx_color(cp->r, cp->g, cp->b);
}

static inline void emu_draw_rect(form_t * r, unsigned color)
{
    emu_set_color(color);
    gfx_rect(r->p.x, r->p.y, r->s.w, r->s.h);
}
