#include "emu_graphic.h"
#include "emu_fb_lcd.h"
#include "lcd_fb.h"
#include "dp.h"

LCD_FB_CREATE(96, 68);

int main()
{
    dpn("test emu fb mono graphic - 0 deg rotate, msb top");

    emu_fb_lcd_cfg_t cfg = {
        .scale = 6,
        .px_gap = 1,
        .border = 10,
        .bg_color = 0x5f7537,
        .on_color = 0,
        .off_color = 0x526137,
        .msb_top = 1,
        .flip_180 = 0,
        .px_byte_debug = 1,
        .px_byte_debug_color = 0x112233
    };
    emu_lcd_mono_init(&cfg);

    lcd_clear();

    dpn(" rect horizontal. left top (5, 10) - size (40, 20)");
    lcd_rect(5, 10, 40, 20, 1);
    lcd_refresh();

    emu_graphic_loop(0);
    return 0;
}