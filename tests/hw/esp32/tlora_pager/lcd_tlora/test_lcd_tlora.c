#include "esp32_gpio.h"
#include "esp32_spi.h"
#include "esp32_pwm.h"

#include "lcd_spi.h"

#include "api_lcd_color.h"
// #include "lcd_text_color.h"
#include "dbg_usb_cdc_acm.h"
#include "tlora_hw.h"

#include "dp.h"

int main(void)
{
    init_spi(&spi);
    init_lcd_hw(&lcd_cfg);
    init_lcd(&lcd_cfg);
    lcd_bl(5);

    lcd_select();

    lcd_rect(0, 0, 100, 40, 0x81F8);
    lcd_rect(100, 100, 100, 40, 0x8F10);

    lcd_rect(20, 80, 100, 10, COLOR(0xFF0000));
    lcd_rect(30, 100, 100, 10, COLOR(0x00FF00));
    lcd_rect(40, 120, 100, 10, COLOR(0x0000FF));

    while (1) {};
}


