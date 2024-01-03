#include "widget_emu_lcd.h"
#include "emu_common.h"
#include "api_lcd.h"


static form_t * emu_lcd_form;
static widget_emu_lcd_cfg_t * emu_lcd_cfg;

static unsigned calc(void * cfg, ui_ctx_t * node_ctx)
{
    emu_lcd_cfg = (widget_emu_lcd_cfg_t *)cfg;
    emu_lcd_form = &node_ctx->f;

    int px_size = emu_lcd_cfg->scale;
    int px_step = px_size + emu_lcd_cfg->px_gap;

    for (int c = 0; c < 2; c++) {
        node_ctx->f.s.ca[c] =
            // результирующий размер каждого пикселя
            px_size * emu_lcd_cfg->size.ca[c] +

            // результирующий размер всех зазоров между пикселями
            (emu_lcd_cfg->px_gap + (emu_lcd_cfg->size.ca[c] - 1)) +

            // размер рамки по краям
            (2 * emu_lcd_cfg->border)
        ;
    }

    return sizeof(ui_ctx_t);
};

static void draw(void * cfg, void * icfg, ui_ctx_t * node_ctx) {
    emu_draw_rect(emu_lcd_form, emu_lcd_cfg->bg_color);
};

widget_t __widget_emu_lcd = {
    .calc = calc,
    .draw = draw
};

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
    emu_draw_rect(&px, color);
}

void lcd_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned color)
{
    for (unsigned rxi = 0; rxi < w; rxi++) {
        for (unsigned ryi = 0; ryi < h; ryi++) {
            emu_lcd_px(x + rxi, y + ryi, color);
        }
    }
}

void lcd_bitmap(unsigned x, unsigned y, unsigned w, unsigned h, unsigned scale, void * buf)
{
    // можно подумать и както сростить с fb


}
