#include "color_type.h"
#include "emu_lcd.h"

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
