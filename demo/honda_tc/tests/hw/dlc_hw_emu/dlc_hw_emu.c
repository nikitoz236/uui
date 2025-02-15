#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "periph_gpio.h"

#include "systick.h"
#include "stm_pwm.h"
#include "stm_usart.h"

#include "delay_blocking.h"
#include "dp.h"

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

const pwm_cfg_t buz_cfg = {
    .ch = 1,
    .freq = 1000,
    .max_val = 200,
    .pclk = PCLK_TIM14,
    .tim = TIM14,
    .gpio = &(gpio_t){
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 4
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
            .af = 4
        }
    }
};

const usart_cfg_t debug_usart = {
    .usart = USART1,
    .default_baud = 115200,
    .rx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 10
        },
        .cfg = {
            .mode = GPIO_MODE_INPUT,
            .pull = GPIO_PULL_NONE,
            .af = 1
        }
    },
    .tx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 9
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
            .af = 1
        }
    },
    .pclk = PCLK_USART1,
    .irqn = USART1_IRQn
};

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&debug_usart, s, len);
}

int main()
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t)PCLK_GPIOA, 1);
    pclk_ctrl(&(pclk_t)PCLK_GPIOB, 1);

    // init_gpio(&pwr_ctrl);
    // gpio_set_state(&pwr_ctrl, 0);

    init_systick();
    __enable_irq();

    init_pwm(&buz_cfg);
    pwm_set_freq(&buz_cfg, 700);
    pwm_set_ccr(&buz_cfg, 10);
    delay_ms(100);
    pwm_set_freq(&buz_cfg, 750);
    delay_ms(100);
    pwm_set_ccr(&buz_cfg, 0);

    usart_set_cfg(&debug_usart);

    dpn("Honda DLC emulator");

    while (1) {};

    return 0;
}
