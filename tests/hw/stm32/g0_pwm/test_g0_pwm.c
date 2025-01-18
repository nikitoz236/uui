#include "periph_header.h"
#include "stm32g0x_rcc.h"
#include "stm32f0x_gpio.h"
#include "stm32g0x_pclk.h"
#include "irq_vectors.h"

const hw_rcc_cfg_t rcc_cfg = {
    .hse_val = 8000000,
    .pll_src = PLL_SRC_HSE,
    .pll_div = 1,
    .pll_mul = 8,
    .sysclk_src = SYSCLK_SRC_PLL,
    .hclk_div = HCLK_DIV1,
    .apb_div = {
        [PCLK_BUS_APB1] = APB_DIV1,
    }
};

const gpio_pin_cfg_t pin_cfg = {
    .gpio = {
        .port = GPIO_PORT_A,
        .pin = 2
    },
    .cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_HIGH,
        .type = GPIO_TYPE_PP,
    }
};

unsigned state = 0;
void tim_irq_handler(void)
{
    TIM1->SR &= ~TIM_SR_UIF;
    state = !state;
    gpio_set_state(&pin_cfg.gpio, state);
}

int main(void)
{
    pclk_ctrl(&PCLK_PWR, 1);

    pclk_ctrl(&PCLK_FLASH, 1);
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&PCLK_GPIOA, 1);
    gpio_configure(&pin_cfg);


    pclk_ctrl(&PCLK_TIM1, 1);

    TIM1->ARR = 500 - 1;
    TIM1->PSC = 1000 - 1;
    TIM1->DIER |= TIM_DIER_UIE;

    NVIC_SetHandler(TIM1_BRK_UP_TRG_COM_IRQn, tim_irq_handler);
    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);

    __enable_irq();

    TIM1->CR1 |= TIM_CR1_CEN;


    while (1) {};

    return 0;
}
