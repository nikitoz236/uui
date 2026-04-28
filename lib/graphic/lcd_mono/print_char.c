#include "print_char.h"
#include "lcd_fb.h"
#include "api_lcd_color.h"

void print_char(char c, xy_t pos, const font_t * font, text_color_t color, unsigned scale)
{
    unsigned use_scale = scale;
    if (use_scale == 0) {
        use_scale = 1;
    }

    if (c == ' ') {
        unsigned w = (unsigned)font->size.w * use_scale;
        unsigned h = (unsigned)font->size.h * use_scale;
        unsigned bg;
        if (color.inverted) {
            bg = 1;
        } else {
            bg = 0;
        }
        lcd_rect(pos.x, pos.y, w, h, bg);
        return;
    }

    unsigned col_step;
    const uint8_t * font_data = font_char_ptr(c, font, &col_step);

    if (font_data) {
        lcd_image_bitmap(pos.x, pos.y, font->size.w, font->size.h, font_data, scale, color.inverted);
    } else {
        static const uint8_t pattern[2][8] = {
            { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA },
            { 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55 }
        };
        for (unsigned rx = 0; rx < (unsigned)font->size.w; rx++) {
            lcd_image_bitmap(pos.x + rx * use_scale, pos.y, 1, font->size.h, &pattern[rx & 1][0], use_scale, color.inverted);
        }
    }
}
