#include "config.h"
#include "rtc.h"
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
    CH7 - LCD_SDO   PA6
    CH8 - LCD_SDI   PA7

 */

const gpio_pin_t debug_gpio_list[] = {
    { GPIO_PORT_C, 13 },
    { GPIO_PORT_B, 8 },
    { GPIO_PORT_B, 9 }
};

const char wf[] = {"Dark spruce forest frowned on either side the frozen waterway. The trees had been stripped by a recent wind of their white covering of frost, and they seemed to lean towards each other, black and ominous, in the fading light."};

int main(void)
{
    //  000 Zero wait state, if 0 < SYSCLK≤ 24 MHz
    //  001 One wait state, if 24 MHz < SYSCLK ≤ 48 MHz
    //  010 Two wait states, if 48 MHz < SYSCLK ≤ 72 MHz
    FLASH->ACR |= FLASH_ACR_LATENCY * 2;
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    hw_rcc_apply_cfg(&hw_rcc_cfg);

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
    AFIO->MAPR |= AFIO_MAPR_TIM1_REMAP_PARTIALREMAP;
    AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_PARTIALREMAP;

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    const hw_pclk_t dma_pclk = {
        .mask = RCC_AHBENR_DMA1EN,
        .bus = PCLK_BUS_AHB
    };

    hw_rcc_pclk_ctrl(&dma_pclk, 1);
    init_systick();
    __enable_irq();

    init_lcd_hw(&lcd_cfg);
    lcd_bl(4);
    init_lcd(&lcd_cfg);

    lcd_rect(4, 5, 310, 228, 0x1234);
    lcd_rect(34, 25, 45, 28, 0x4567);

    extern font_t font_5x7;
    lcd_text_color_print("Hey bitch!", &(xy_t){ 50, 60 }, &(lcd_text_cfg_t){.font = &font_5x7, .gaps = {2, 2}, .scale = 2, .text_size = {16, 1}}, &(color_scheme_t){ .bg = 0xABCD, .fg = 0x5432}, 0, 0, 0 );

    while (1) {};


/*
    while (1) {};
    for (unsigned i = 0; i < ARRAY_SIZE(debug_gpio_list); i++) {
        const gpio_cfg_t cfg = {
            .mode = GPIO_MODE_OUTPUT,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
            .pull = GPIO_PULL_NONE,
        };
        gpio_set_cfg(&debug_gpio_list[i], &cfg);
        gpio_set_state(&debug_gpio_list[i], 0);
    }

    for (unsigned i = 0; i < 4; i++) {
        gpio_set_state(&debug_gpio_list[0], 1);
        test_delay_ms(100);
        gpio_set_state(&debug_gpio_list[0], 0);
        test_delay_ms(100);
    }

*/

    // init_rtc();

    /*

    надо проверить ходят ли они ? сбрасываются ли без питания ? читать время - показывать.

    потом ставить время

    надо логгинг

    dp()
    dpn
    dpdn
    dpwdwn

    db(str, "rtc time:", dec, 6, rtc_s, rn );

    */

    unsigned rtc_last = 0;

    mstimer_t led_flash_timer = mstimer_with_timeout(500);
    unsigned led_state = 0;


    while (1) {
        unsigned rtc_s = rtc_get_time_s();
        if (rtc_last != rtc_s) {
            rtc_last = rtc_s;
            dp("rtc time: ");
            dpdz(rtc_s, 10);
            dn();
        }

    }

    return 0;
}