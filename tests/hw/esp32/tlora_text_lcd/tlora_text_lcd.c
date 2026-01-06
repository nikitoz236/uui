#include "esp32_gpio.h"
#include "esp32_spi.h"
#include "esp32_pwm.h"
#include "esp32_systime.h"

#include "lcd_spi.h"

#include "api_lcd_color.h"
#include "lcd_text_color.h"

#include "esp32_i2c.h"
#include "tca8418_kbd.h"
#include "bq25896.h"

#include "dbg_usb_cdc_acm.h"

#define DP_NOTABLE
#include "dp.h"

void __debug_usart_tx_data(const char * s, unsigned len)
{
    dbg_usb_cdc_acm_tx(s, len);
}

spi_cfg_t lcd_spi = {
    .spi = &GPSPI2,
    .gpio_list = &(gpio_list_t){
        .cfg = { .mode = GPIO_MODE_SIG_IO },
        .count = 2,
        .pin_list = {
            [SPI_PIN_MOSI] = { .pin = 34, .signal = FSPID_OUT_IDX },
            [SPI_PIN_SCK] = { .pin = 35, .signal = FSPICLK_OUT_IDX }
        }
    }
};

const lcd_cfg_t lcd_cfg = {
    .no_reset = 1,
    .ctrl_lines = &(gpio_list_t){
        .cfg = { .mode = GPIO_MODE_OUT },
        .count = 1,
        .pin_list = {
            [LCD_DC] = { .pin = 37 }
        }
    },
    .spi_dev = {
        .cs_pin = &(gpio_t){
            .cfg = { .mode = GPIO_MODE_OUT },
            .pin = { .pin = 38 }
        },
        .spi = &lcd_spi,
    },
    // .bl = & (pwm_cfg_t) {
    //     .gpio = & (gpio_t) {
    //         .cfg = { .mode = GPIO_MODE_SIG_OUT },
    //         .pin = { .pin = 38, .signal = LEDC_LS_SIG_OUT0_IDX }
    //     },
    //     .out_ch = 0,
    //     .tim_ch = 0,
    //     .duty_res = 6
    // },
    .gcfg = {
        .height = 222,
        .width = 480,
        .x_offset = 0,
        .y_offset = 49,
        .x_flip = 1,
        .y_flip = 1,
        .bgr = 1

    }
};


i2c_cfg_t i2c_bus_cfg = {
    .dev = &I2C0,
    .i2c_pclk = SYSTEM_I2C_EXT0_CLK_EN_S,
    .freq = 400000,
    .pin_list = &(gpio_list_t){
        .count = 2,
        .cfg = {
            .mode = GPIO_MODE_SIG_IO,
            .pu = 1,
            .od = 1
        },
        .pin_list = {
            { .pin = 2, .signal = I2CEXT0_SCL_IN_IDX },
            { .pin = 3, .signal = I2CEXT0_SDA_IN_IDX }
        }
    }
};

gpio_t kbd_irq_line = {
    .cfg = { .mode = GPIO_MODE_IN, .pu = 1 },
    .pin = { .pin = 6 }
};



gpio_t bl = {
    .cfg = { .mode = GPIO_MODE_OUT },
    .pin = { .pin = 42 }
};

void init_bl(void)
{
    init_gpio(&bl);
}

#include "delay_blocking.h"

void bl_set(unsigned lvl)
{
    if (lvl >= 16) {
        lvl = 0;
    }

    if (lvl) {
        gpio_set_state(&bl, 1);
        lvl--;
    }

    while (lvl--) {
        delay_us(2);
        gpio_set_state(&bl, 0);
        delay_us(2);
        gpio_set_state(&bl, 1);
    }
}

form_t display_lcd_cfg_form(lcd_cfg_t * cfg)
{
    form_t f = {
        .p = { .x = 0, .y = 0 },
        .s = { .w = cfg->gcfg.width, .h = cfg->gcfg.height }
    };
    return f;
}

color_scheme_t cs = {.bg = 0, .fg = 0xA234};
tptr_t tp;

const char sym[] = {
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '\n',
    0,   'z', 'x', 'c', 'v', 'b', 'n', 'm', 0,
    '\b', ' '
};

void kbd_change_handler(unsigned num, unsigned state)
{
    dp("key : "); dpd(num, 2); dp(" = "); dpd(state, 1); dn();

    if (num == 0x15) {
        bq25896_power_off();
    }

    if (state) {
        uint8_t c = sym[num - 1];
        if (c) {
            dp("  char : "); dpx(c, 1); dn();
            // __debug_usart_tx_data(&c, 1);
            // lcd_color_tptr_print(&tp, &c, cs, 1);
            // text_ptr_next_char(&tp);
        }
    }
}
#include "soc/usb_serial_jtag_struct.h"
#include "str_val.h"

void lcd_print_hex(uint32_t val, xy_t pos)
{
    char str[10];
    hex_to_str(&val ,str, 4);
    text_ptr_set_char_pos(&tp, pos);
    lcd_color_tptr_print(&tp, str, cs, 8);
}

void lcd_print_dump_val(unsigned idx, uint32_t val)
{
    xy_t pos = { .x = (idx & 3) * 10, .y = (idx >> 2) };
    lcd_print_hex(val, pos);
}

#include "timers_32.h"
timer_32_t tim;

int main(void)
{
    // dpn("TCA8418 t lora keyboard with display text print");

    uint32_t s = USB_SERIAL_JTAG.out_ep1_st.val;
    // dpx(s, 4); dn();

    init_gpio(&kbd_irq_line);
    init_i2c(&i2c_bus_cfg);
    init_tca8418();

    init_bl();
    // for (unsigned i = 0; i <= 16; i++) {
    //     bl_set(i);
    //     dp("bl lvl "); dpd(i, 2); dn();
    //     delay_ms(2000);
    // }

    bl_set(9);

    init_lcd_hw(&lcd_cfg);
    init_lcd(&lcd_cfg);

    // lcd_bl(30);

    // lcd_rect(0, 0, 100, 40, 0x81F8);
    // lcd_rect(100, 100, 100, 40, 0x8F10);

    // lcd_rect(20, 80, 100, 10, COLOR(0xFF0000));
    // lcd_rect(30, 100, 100, 10, COLOR(0x00FF00));
    // lcd_rect(40, 120, 100, 10, COLOR(0x0000FF));

    extern font_t font_5x7;

    lcd_font_cfg_t fcfg = {
        .font = &font_5x7,
        .gaps = { .x = 4, .y = 4 },
        .scale = 1
    };

    form_t lcdf = display_lcd_cfg_form(&lcd_cfg);
    xy_t lim = lcd_text_char_places(&fcfg, lcdf.s);

    text_ptr_init(&tp, &fcfg, lcdf.p, &lim);

    uint32_t sc[sizeof(USB_SERIAL_JTAG) / 4];
    uint32_t * pp = (uint32_t *)&USB_SERIAL_JTAG;

    for (unsigned i = 0; i < (sizeof(USB_SERIAL_JTAG) / 4); i++) {
        uint32_t v = pp[i];
        sc[i] = v;
        lcd_print_dump_val(i, v);
    }

    // lcd_color_tptr_print(&tp, str, cs, 0);
    // text_ptr_next_str(&tp);
    // lcd_color_tptr_print(&tp, "lol>", cs, 0);




    unsigned prev_cnt = 0;

    unsigned timcnt = 0;
    t32_run(&tim, systimer_ms(0), 1000);

    while (1) {
        // extern volatile uint32_t usb_write_cnt;
        // if (prev_cnt != usb_write_cnt) {
        //     prev_cnt = usb_write_cnt;
        //     lcd_print_hex(prev_cnt, (xy_t){ .x = 0, .y = 10});
        // }
        // for (unsigned i = 0; i < (sizeof(USB_SERIAL_JTAG) / 4); i++) {
        //     uint32_t v = pp[i];
        //     if (sc[i] != v) {
        //         sc[i] = v;
        //         lcd_print_dump_val(i, v);
        //     }
        // }
        // if (USB_SERIAL_JTAG.int_raw.in_token_rec_in_ep1_int_raw) {
        //     USB_SERIAL_JTAG.int_clr.in_token_rec_in_ep1_int_clr = 1;
        //     cnt++;
        //     lcd_print_hex(cnt, (xy_t){ .x = 0, .y = 10});
        // }

        if (t32_is_over(&tim, systimer_ms(0))) {
            t32_extend(&tim, 1000);
            timcnt++;
            lcd_print_hex(timcnt, (xy_t){ .x = 0, .y = 12});
        }

        if (gpio_get_state(&kbd_irq_line) == 0) {
            dpn("kbd irq detect");
            tca8418_poll_kp_fifo(kbd_change_handler);
        }
    }
}


