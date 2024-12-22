#include "emu_lcd.h"
#include "emu_graphic.h"

static form_t * emu_lcd_form;
static emu_lcd_cfg_t * emu_lcd_cfg;

void emu_lcd_init(emu_lcd_cfg_t * cfg, form_t * f)
{
    emu_lcd_cfg = cfg;
    emu_lcd_form = f;

    int px_size = emu_lcd_cfg->scale;
    int px_step = px_size + emu_lcd_cfg->px_gap;

    for (int c = 0; c < 2; c++) {
        emu_lcd_form->s.ca[c] = (
            // результирующий размер каждого пикселя
            px_size * emu_lcd_cfg->size.ca[c] +

            // результирующий размер всех зазоров между пикселями
            (emu_lcd_cfg->px_gap * (emu_lcd_cfg->size.ca[c] - 1)) +

            // размер рамки по краям
            (2 * emu_lcd_cfg->border)
        );
    }
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

void lcd_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned color)
{
    for (unsigned rxi = 0; rxi < w; rxi++) {
        for (unsigned ryi = 0; ryi < h; ryi++) {
            emu_lcd_px(x + rxi, y + ryi, color);
        }
    }
}

void lcd_image(unsigned x, unsigned y, unsigned w, unsigned h, unsigned scale, lcd_color_t * buf)
{
    if (scale == 0) {
        scale = 1;
    }
    for (unsigned rxi = 0; rxi < (w * scale); rxi += scale) {
        for (unsigned ryi = 0; ryi < (h * scale); ryi += scale) {
            lcd_color_t color = *buf++;
            if (scale > 1) {
                lcd_rect(x + rxi, y + ryi, scale, scale, color);
            } else {
                emu_lcd_px(x + rxi, y + ryi, color);
            }
        }
    }
}
