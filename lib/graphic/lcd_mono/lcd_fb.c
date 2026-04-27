#include "lcd_fb.h"

extern const fb_desc_t __lcd_fb_desc;

void lcd_clear(void)
{
    const fb_desc_t * d = &__lcd_fb_desc;
    for (lcd_fb_size_t i = 0; i < d->fb_len; i++) {
        d->fb_ctx->fb[i] = 0;
    }
}

void lcd_rect(unsigned x, unsigned y, unsigned w, unsigned h, lcd_color_t color)
{
    const fb_desc_t * d = &__lcd_fb_desc;

    if (x >= (unsigned)d->w || y >= (unsigned)d->h) {
        return;
    }
    if (x + w > (unsigned)d->w) {
        w = d->w - x;
    }
    if (y + h > (unsigned)d->h) {
        h = d->h - y;
    }

#if defined LCD_FLIP_180
    x = d->w - w - x;
    y = d->h - h - y;
#endif

    unsigned p_min = y >> 3;
    unsigned p_max = (y + h - 1) >> 3;
    unsigned dn_mask;

#if defined LCD_TOP_LSB
    dn_mask = 0xFF >> ((-y - h) & 7);
#elif defined LCD_TOP_MSB
    dn_mask = 0xFF << ((-y - h) & 7);
#else
    #error lcd top pixel
#endif

    for (unsigned p = p_min; p <= p_max; p++) {
        unsigned mask = 0xFF;
        if (p == p_min) {
#if defined LCD_TOP_LSB
            mask = 0xFF << (y & 7);
#else
            mask = 0xFF >> (y & 7);
#endif
        }
        if (p == p_max) {
            mask &= dn_mask;
        }

        uint8_t * fb = &d->fb_ctx->fb[(lcd_fb_size_t)p * d->w + x];
        unsigned wtmp = w;
        if (mask == 0xFF) {
            if (color) {
                while (wtmp--) {
                    *fb++ = 0xFF;
                }
            } else {
                while (wtmp--) {
                    *fb++ = 0;
                }
            }
        } else if (color) {
            while (wtmp--) {
                *fb++ |= mask;
            }
        } else {
            unsigned inv = ~mask;
            while (wtmp--) {
                *fb++ &= inv;
            }
        }
    }
}
