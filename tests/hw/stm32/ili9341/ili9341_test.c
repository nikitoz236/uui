#include "config.h"
// #include "rtc.h"
#include "str_val.h"
#include "array_size.h"
#include "systick.h"
#include "mstimer.h"
#include "delay_blocking.h"
#include "lcd_spi.h"
#include "api_lcd_color.h"
#include "lcd_text_color.h"

/*
    конфигурация логанализатора
    CH1 - USART_TX  PA9
    CH2 - LED       PC13
    CH3 - CAN_RX    PB8
    CH4 - CAN_TX    PB9
    CH5 - LCD_DC    PA11
    CH6 - LCD_SCL   PA5
    CH7 - LCD_SDO   PA6     / RST
    CH8 - LCD_SDI   PA7

 */

const gpio_t led = {
    .gpio = { GPIO_PORT_C, 13 },
    .cfg = (gpio_cfg_t){
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_HIGH,
        .type = GPIO_TYPE_PP
    }
};

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t)PCLK_AFIO, 1);

    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
    AFIO->MAPR |= AFIO_MAPR_TIM1_REMAP_PARTIALREMAP;
    AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_PARTIALREMAP;

    pclk_ctrl(&(pclk_t)PCLK_IOPA, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPB, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPC, 1);

    pclk_ctrl(&(pclk_t)PCLK_DMA1, 1);

    init_systick();
    __enable_irq();

    init_gpio(&led);

    init_lcd_hw(&lcd_cfg);
    lcd_bl(5);

    init_lcd(&lcd_cfg);

    lcd_rect(4, 5, 310, 228, COLOR(0xFF0000));
    lcd_rect(34, 25, 45, 28, 0x4567);

    lcd_rect(34, 129, 45, 28, 0);

    lcd_rect(94, 129, 45, 28, 0xFFFF);

    lcd_rect(180, 129, 45, 28, COLOR(0x00FF00));

    lcd_rect(250, 129, 45, 28, COLOR(0x0000FF));

    extern font_t font_5x7;
    lcd_text_color_print(
        "Hey bitch!",
        &(xy_t){.x = 50, .y = 60 },
        &(lcd_text_cfg_t){
            .font = &font_5x7,
            .gaps = { .x = 2, .y = 2 },
            .scale = 2,
            .text_size = { .w = 16, .h = 1 }
        },
        &(color_scheme_t){ .bg = COLOR(0xAA0000), .fg = COLOR(0x1122AA)},
        0, 0, 0 );

    while (1) {};

    return 0;
}
