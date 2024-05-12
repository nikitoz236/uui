#include "config.h"

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

    struct hw_pclk hw_pclk_gpio = {
        .bus = PCLK_BUS_APB2,
        .mask = RCC_APB2ENR_IOPCEN,
    };

    hw_rcc_pclk_ctrl(&hw_pclk_gpio, 1);

    gpio_cfg_t led_pin_cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_LOW,
        .type = GPIO_TYPE_PP,
        .pull = GPIO_PULL_NONE,
    };

    gpio_pin_t led_pin = {
        .port = 2,
        .pin = 13,
    };

    gpio_set_cfg(&led_pin, &led_pin_cfg);
    gpio_set_state(&led_pin, 1);

    while (1) {};

    return 0;
}
