#include "esp32_gpio.h"
#include "esp32_spi.h"
#include "esp32_pwm.h"

#include "lcd_spi.h"


#include "esp32_i2c.h"
#include "tca8418_kbd.h"

#include "dbg_usb_cdc_acm.h"

#define DP_NOTABLE
#include "dp.h"


#include "delay_blocking.h"
#include "forms.h"


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

    uint32_t s = USB_SERIAL_JTAG.out_ep1_st.val;
    // dpx(s, 4); dn();

    init_gpio(&kbd_irq_line);
    init_i2c(&i2c_bus_cfg);
    init_tca8418();

    init_bl();
    bl_set(9);

    init_lcd_hw(&lcd_cfg);
    init_lcd(&lcd_cfg);

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


