#include "emu_fb_lcd.h"
#include "emu_graphic.h"
#include "emu_lcd.h"
#include "lcd_fb.h"

extern const fb_desc_t __lcd_fb_desc;

static const emu_fb_lcd_cfg_t * saved_cfg;

void emu_lcd_mono_init(const emu_fb_lcd_cfg_t * cfg)
{
    saved_cfg = cfg;

    static emu_lcd_cfg_t base;
    base = (emu_lcd_cfg_t){
        .size = { .w = __lcd_fb_desc.w, .h = __lcd_fb_desc.h },
        .scale = cfg->scale,
        .px_gap = cfg->px_gap,
        .border = cfg->border,
        .bg_color = cfg->bg_color,
    };
    emu_init(&base);
}

void lcd_refresh(void)
{
    const fb_desc_t * d = &__lcd_fb_desc;
    const emu_fb_lcd_cfg_t * cfg = saved_cfg;

    for (coord_t y = 0; y < d->h; y++) {
        for (coord_t x = 0; x < d->w; x++) {
            coord_t fx = x;
            coord_t fy = y;
            if (cfg->flip_180) {
                fx = d->w - 1 - x;
                fy = d->h - 1 - y;
            }
            unsigned p = fy >> 3;
            unsigned pxn = fy & 7;
            unsigned mask;
            if (cfg->msb_top) {
                mask = 0x80 >> pxn;
            } else {
                mask = 1 << pxn;
            }
            unsigned bit = d->fb_ctx->fb[p * d->w + fx] & mask;
            if (bit) {
                emu_lcd_px(x, y, cfg->on_color);
            } else {
                emu_lcd_px(x, y, cfg->off_color);
            }
        }
    }

    if (cfg->px_byte_debug) {
        unsigned p_max = ((unsigned)d->h + 7) / 8;
        for (unsigned p = 0; p + 1 < p_max; p++) {
            unsigned y;
            if (cfg->flip_180) {
                y = (unsigned)d->h - 1 - (p + 1) * 8;
            } else {
                y = p * 8 + 7;
            }
            emu_lcd_byte_gap_h(y, cfg->px_byte_debug_color);
        }
    }
}
