#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "stm_pwm.h"

#include "periph_gpio.h"

const rcc_cfg_t rcc_cfg = {
    .hse_val = 8000000,
    .pll_src = PLL_SRC_PREDIV,
    .pll_prediv = 1,
    .pll_mul = 9,
    .sysclk_src = SYSCLK_SRC_PLL,
    .hclk_div = HCLK_DIV1,
    .apb_div = {
        APB_DIV2,
        APB_DIV1
    }
};

const pwm_cfg_t pwm_cfg = {
    .ch = 3 - 1,
    .freq = 1000,
    .max_val = 10,
    .pclk = PCLK_TIM4,
    .tim = TIM4,
    .gpio = &(const gpio_t){
        .gpio = {
            .port = GPIO_PORT_B,
            .pin = 8,
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
        }
    }
};

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&PCLK_AFIO, 1);

    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    pclk_ctrl(&PCLK_IOPA, 1);
    pclk_ctrl(&PCLK_IOPB, 1);
    pclk_ctrl(&PCLK_IOPC, 1);

    pclk_ctrl(&PCLK_DMA1, 1);
    __enable_irq();

    init_pwm(&pwm_cfg);
    pwm_set_ccr(&pwm_cfg, 5);

    while (1) {};

    return 0;
}
