#include "emu_lcd.h"
#include "forms.h"
#include "gfx.h"

static form_t * emu_lcd_form;
static emu_lcd_cfg_t * emu_lcd_cfg;

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

static void emu_graphic_init_window(xy_t size)
{
    gfx_open((int)size.w, (int)size.h, "emu");
}

char emu_routine(void)
{
    return gfx_routine();
}

static void emu_lcd_init_form(emu_lcd_cfg_t * cfg, form_t * f)
{
    emu_lcd_cfg = cfg;
    emu_lcd_form = f;

    unsigned px_size = emu_lcd_cfg->scale;
    unsigned px_step = px_size + emu_lcd_cfg->px_gap;

    for (dimension_t d = 0; d < DIMENSION_COUNT; d++) {
        emu_lcd_form->s.ca[d] = (
            // результирующий размер каждого пикселя
            px_size * emu_lcd_cfg->size.ca[d] +

            // результирующий размер всех зазоров между пикселями
            (emu_lcd_cfg->px_gap * (emu_lcd_cfg->size.ca[d] - 1)) +

            // размер рамки по краям
            (2 * emu_lcd_cfg->border)
        );
    }
}

void emu_init(emu_lcd_cfg_t * cfg)
{
    static form_t lcd_form = {};

    emu_lcd_init_form(cfg, &lcd_form);

    emu_graphic_init_window(lcd_form.s);

    emu_lcd_clear();
}

void emu_lcd_clear(void)
{
    emu_graphic_rect(emu_lcd_form, emu_lcd_cfg->bg_color);
}

void emu_lcd_px(int x, int y, int color)
{
    int px_size = emu_lcd_cfg->scale;
    int px_step = px_size + emu_lcd_cfg->px_gap;
    form_t px = {
        .s = {
            .w = px_size,
            .h = px_size,
        },
        .p = {
            .x = emu_lcd_form->p.x + emu_lcd_cfg->border + (x * px_step),
            .y = emu_lcd_form->p.y + emu_lcd_cfg->border + (y * px_step),
        }
    };
    emu_graphic_rect(&px, color);
}

void emu_lcd_byte_gap_h(unsigned y, unsigned color)
{
    unsigned px_size = emu_lcd_cfg->scale;
    unsigned px_step = px_size + emu_lcd_cfg->px_gap;
    unsigned screen_w = px_size * emu_lcd_cfg->size.w +
                        emu_lcd_cfg->px_gap * (emu_lcd_cfg->size.w - 1);
    form_t line = {
        .s = {
            .w = screen_w,
            .h = emu_lcd_cfg->px_gap,
        },
        .p = {
            .x = emu_lcd_form->p.x + emu_lcd_cfg->border,
            .y = emu_lcd_form->p.y + emu_lcd_cfg->border + (y * px_step) + px_size,
        }
    };
    emu_graphic_rect(&line, color);
}
