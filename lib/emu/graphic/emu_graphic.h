#pragma once
#include <stdint.h>
#include <unistd.h>
#include "gfx.h"
#include "forms.h"

static inline void emu_graphic_init(unsigned w, unsigned h)
{
    gfx_open(w, h, "uui graphic emulator");
}

static inline void emu_graphic_init_xy(xy_t size)
{
    gfx_open(size.w, size.h, "uui graphic emulator");
}

static inline void emu_graphic_rect(form_t * r, unsigned color)
{
    struct color {
        int b : 8;
        int g : 8;
        int r : 8;
    };
    struct color * cp = (struct color *)&color;
    gfx_color(cp->r, cp->g, cp->b);

    gfx_rect(r->p.x, r->p.y, r->s.w, r->s.h);
}

static inline void emu_graphic_loop(void (*process)(char key))
{
    while (1) {
        char key = gfx_routine();
        if (key == 'q') {
            break;
        }
        if (process) {
            process(key);
        }

        usleep(100000);
    }
}
