
#include "dp.h"

#include "forms.h"
#include "tlora_hw.h"

form_t display_lcd_cfg_form(lcd_cfg_t * cfg)
{
    form_t f = {
        .p = { .x = 0, .y = 0 },
        .s = { .w = cfg->gcfg.width, .h = cfg->gcfg.height }
    };
    return f;
}

#include "soc/usb_serial_jtag_struct.h"
#include "str_val.h"

// void lcd_print_hex(uint32_t val, xy_t pos)
// {
//     char str[10];
//     hex_to_str(&val ,str, 4);
//     text_ptr_set_char_pos(&tp, pos);
//     lcd_color_tptr_print(&tp, str, cs, 8);
// }

// void lcd_print_dump_val(unsigned idx, uint32_t val)
// {
//     xy_t pos = { .x = (idx & 3) * 10, .y = (idx >> 2) };
//     lcd_print_hex(val, pos);
// }

void init_console(form_t * f);
void console_process(void);
void kbd_change_handler(unsigned num, unsigned state);


int main(void)
{
    dpn("TCA8418 t lora keyboard with display text print");

    // uint32_t s = USB_SERIAL_JTAG.out_ep1_st.val;
    // dpx(s, 4); dn();

    init_gpio(&kbd_irq_line);
    init_i2c(&i2c_bus_cfg);
    init_tca8418();

    init_spi(&spi);
    init_lcd_hw(&lcd_cfg);
    lcd_bl(9);
    init_lcd(&lcd_cfg);
    lcd_select();

    form_t lcdf = display_lcd_cfg_form(&lcd_cfg);
    init_console(&lcdf);

    while (1) {
        console_process();
        if (gpio_get_state(&kbd_irq_line) == 0) {
            dpn("kbd irq detect");
            tca8418_poll_kp_fifo(kbd_change_handler);
        }
    }
}


