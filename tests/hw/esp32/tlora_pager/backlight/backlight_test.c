#include "tlora_hw.h"

#include "forms.h"
#include "font_config.h"
#include "text_pointer.h"
#include "lcd_text_color.h"
#include "api_lcd_color.h"
#include "str_val.h"

unsigned brightness = 9;

extern font_t font_5x7;

const lcd_font_cfg_t fcfg = {
    .font = &font_5x7,
    .gaps = { .x = 4, .y = 4 },
    .scale = 2
};

color_scheme_t cs_val  = { .fg = 0xFFFF, .bg = 0x0000 };
color_scheme_t cs_help = { .fg = 0x8410, .bg = 0x0000 };

tptr_t tp_val;
tptr_t tp_help;

form_t display_lcd_cfg_form(lcd_cfg_t * cfg)
{
    form_t f = {
        .p = { .x = 0, .y = 0 },
        .s = { .w = cfg->gcfg.width, .h = cfg->gcfg.height }
    };
    return f;
}

void draw_value(void)
{
    char buf[4];
    dec_to_str_right_aligned(brightness, buf, 2, 0);

    text_ptr_set_char_pos(&tp_val, (xy_t){ .x = 0, .y = 0 });
    lcd_color_tptr_print(&tp_val, "BL: ", cs_val, 4);
    lcd_color_tptr_print(&tp_val, buf, cs_val, 2);
    lcd_color_tptr_print(&tp_val, "  ", cs_val, 2);
}

void draw_help(void)
{
    text_ptr_set_char_pos(&tp_help, (xy_t){ .x = 0, .y = 0 });
    lcd_color_tptr_print(&tp_help, "[u] up  [d] down", cs_help, 17);
}

void ui_init(form_t * f)
{
    xy_t chars = fcfg_text_char_places(&fcfg, f->s);

    tp_val = text_ptr_create((tf_t){
        .fcfg = &fcfg,
        .pos  = f->p,
        .lim  = { .x = chars.x, .y = 1 }
    });

    xy_t help_pos = {
        .x = f->p.x,
        .y = f->p.y + fcfg_text_size_px(&fcfg, (xy_t){ .x = 0, .y = 1 }).y
    };

    tp_help = text_ptr_create((tf_t){
        .fcfg = &fcfg,
        .pos  = help_pos,
        .lim  = { .x = chars.x, .y = 1 }
    });

    lcd_rect(0, 0, f->s.w, f->s.h, 0x0000);
    draw_value();
    draw_help();
}

void kbd_change_handler(unsigned num, unsigned state)
{
    if (!state) {
        return;
    }

    num--;

    unsigned prev = brightness;

    if (num == TLORA_KBD_KEY_U) {
        if (brightness < 15) {
            brightness++;
        }
    } else if (num == TLORA_KBD_KEY_D) {
        if (brightness > 0) {
            brightness--;
        }
    }

    if (brightness == prev) {
        return;
    }

    if (brightness) {
        lcd_bl(16 - brightness);
    } else {
        lcd_bl(0);
    }
    draw_value();

    dp("brightness = "); dpd(brightness, 2); dn();
}


int main(void)
{
    dpn("backlight test");

    init_gpio(&kbd_irq_line);
    init_i2c(&i2c_bus_cfg);
    init_tca8418();

    init_lcd_hw(&lcd_cfg);
    lcd_bl(16 - brightness);
    init_lcd(&lcd_cfg);

    form_t lcdf = display_lcd_cfg_form(&lcd_cfg);
    ui_init(&lcdf);

    while (1) {
        if (gpio_get_state(&kbd_irq_line) == 0) {
            tca8418_poll_kp_fifo(kbd_change_handler);
        }
    }
}
