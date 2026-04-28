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

static void bitmask_slow(unsigned x, unsigned y, unsigned w, unsigned h, const uint8_t * data, unsigned scale, unsigned inverted)
{
    unsigned col_step = (h + 7) / 8;
    for (unsigned rx = 0; rx < w; rx++) {
        const uint8_t * col = &data[rx * col_step];
        for (unsigned ry = 0; ry < h; ry++) {
            unsigned bit = (col[ry >> 3] >> (ry & 7)) & 1;
            bit ^= inverted;
            lcd_rect(x + rx * scale, y + ry * scale, scale, scale, bit);
        }
    }
}

static inline uint8_t bit_reverse_8(uint8_t b)
{
    b = ((b & 0xF0) >> 4) | ((b & 0x0F) << 4);
    b = ((b & 0xCC) >> 2) | ((b & 0x33) << 2);
    b = ((b & 0xAA) >> 1) | ((b & 0x55) << 1);
    return b;
}

static void bitmask_fast(unsigned x, unsigned y, unsigned w, unsigned h, const uint8_t * data, unsigned inverted)
{
    const fb_desc_t * d = &__lcd_fb_desc;
    unsigned col_step = (h + 7) / 8;

    for (unsigned rx = 0; rx < w; rx++) {
        unsigned fb_x = x + rx;
        if (fb_x >= (unsigned)d->w) {
            continue;
        }

        unsigned rem_h = h;
        unsigned byte_idx = 0;
        unsigned y_top = y;

        while (rem_h > 0) {
            unsigned n;
            if (rem_h > 8) {
                n = 8;
            } else {
                n = rem_h;
            }
            uint8_t inner_mask = (1 << n) - 1;
            uint8_t byte_data = data[rx * col_step + byte_idx] & inner_mask;
            if (inverted) {
                byte_data ^= inner_mask;
            }

            unsigned page = y_top >> 3;
            unsigned shift = y_top & 7;

#if defined LCD_TOP_LSB
            uint16_t big = (uint16_t)byte_data << shift;
            uint16_t big_mask = (uint16_t)inner_mask << shift;
            uint8_t low_v = big & 0xFF;
            uint8_t low_m = big_mask & 0xFF;
            uint8_t high_v = big >> 8;
            uint8_t high_m = big_mask >> 8;
#elif defined LCD_TOP_MSB
            uint8_t rev_data = bit_reverse_8(byte_data);
            uint8_t rev_mask = bit_reverse_8(inner_mask);
            uint16_t big = ((uint16_t)rev_data << 8) >> shift;
            uint16_t big_mask = ((uint16_t)rev_mask << 8) >> shift;
            uint8_t low_v = big >> 8;
            uint8_t low_m = big_mask >> 8;
            uint8_t high_v = big & 0xFF;
            uint8_t high_m = big_mask & 0xFF;
#else
            #error lcd top pixel
#endif

            if (page < d->rows && low_m) {
                uint8_t * fb_byte = &d->fb_ctx->fb[page * d->w + fb_x];
                uint8_t cur = *fb_byte;
                uint8_t cleared = cur & ~low_m;
                uint8_t merged = cleared | (low_v & low_m);
                *fb_byte = merged;
            }
            unsigned page_next = page + 1;
            if (page_next < d->rows && high_m) {
                uint8_t * fb_byte = &d->fb_ctx->fb[page_next * d->w + fb_x];
                uint8_t cur = *fb_byte;
                uint8_t cleared = cur & ~high_m;
                uint8_t merged = cleared | (high_v & high_m);
                *fb_byte = merged;
            }

            rem_h -= n;
            byte_idx++;
            y_top += 8;
        }
    }
}

void lcd_image_bitmask(unsigned x, unsigned y, unsigned w, unsigned h, const uint8_t * data, unsigned scale, unsigned inverted)
{
    if (scale == 0) {
        scale = 1;
    }

    #if !defined LCD_FLIP_180
        if (scale == 1) {
            bitmask_fast(x, y, w, h, data, inverted);
            return;
        }
    #endif
    bitmask_slow(x, y, w, h, data, scale, inverted);
}
