#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "periph_gpio.h"

#include "stm_pwm.h"

const rcc_cfg_t rcc_cfg = {
    .hse_val = 8000000,
    .pll_src = PLL_SRC_PREDIV,
    .pll_prediv = 1,
    .pll_mul = 6,
    .sysclk_src = SYSCLK_SRC_PLL,
    .hclk_div = HCLK_DIV1,
    .apb_div = {
        APB_DIV1,
    }
};

const gpio_list_t leds = {
    .cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_HIGH,
        .type = GPIO_TYPE_PP,
    },
    .count = 2,
    .pin_list = (const gpio_pin_t[]){
        {
            .port = GPIO_PORT_A,
            .pin = 8
        },
        {
            .port = GPIO_PORT_A,
            .pin = 11
        }
    }
};

const pwm_cfg_t pwm_led = {
    .ch = 1 - 1,
    .freq = 1000,
    .max_val = 10,
    .pclk = PCLK_TIM1,
    .tim = TIM1,
    .gpio = &(gpio_t){
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 8
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
            .af = 2
        }
    }
};

int main()
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&PCLK_GPIOA, 1);
    // init_gpio_list(&leds);
    // gpio_list_set_state(&leds, 1, 1);

    init_pwm(&pwm_led);
    pwm_set_ccr(&pwm_led, 4);

    while (1) {};

    return 0;
}
