#include "emu_graphic.h"
#include "emu_fb_lcd.h"
#include "lcd_fb.h"
#include "dp.h"

LCD_FB_CREATE(96, 68);

int main()
{
    dpn("test emu graphic mono lcd");

    emu_fb_lcd_cfg_t cfg = {
        .scale = 6,
        .px_gap = 1,
        .border = 10,
        .bg_color = 0x5f7537,
        .on_color = 0,
        .off_color = 0x526137,
        .msb_top = 0,
        .flip_180 = 0,
    };
    emu_lcd_mono_init(&cfg);

    lcd_clear();
    lcd_rect(5, 5, 30, 20, 1);
    lcd_rect(40, 10, 20, 40, 1);
    lcd_rect(10, 30, 70, 5, 1);
    lcd_refresh();

    emu_graphic_loop(0);
    return 0;
}
