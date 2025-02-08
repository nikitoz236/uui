#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "stm_pwm.h"
#include "systick.h"
#include "delay_blocking.h"
#include "array_size.h"

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
    .ch = 3,
    .freq = 4000,
    .max_val = 200,
    .pclk = PCLK_TIM3,
    .tim = TIM3,
    .gpio = &(const gpio_t){
        .gpio = {
            .port = GPIO_PORT_B,
            .pin = 0,
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
        }
    }
};

struct player {
    uint16_t duration;
    uint16_t freq;
    uint8_t lvl;
};

// struct player sound[] = {
//     {100, 4000, 5},
//     {100, 6000, 9},
//     {100, 2000, 3},
// };

struct player sound[] = {
    // {200, 2000, 5},
    // {300, 2000, 9},
    // {200, 5000, 3},
    {100, 230, 100},
    {100, 280, 100},
    {100, 800, 100},
    // {200, 800, 18},


};

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t)PCLK_AFIO, 1);

    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    pclk_ctrl(&(pclk_t)PCLK_IOPA, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPB, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPC, 1);

    pclk_ctrl(&(pclk_t)PCLK_DMA1, 1);
    init_systick();

    __enable_irq();

    init_pwm(&pwm_cfg);
    pwm_set_ccr(&pwm_cfg, 2);

    for (unsigned i = 0; i < ARRAY_SIZE(sound); i++) {
        pwm_set_freq(&pwm_cfg, sound[i].freq);
        pwm_set_ccr(&pwm_cfg, sound[i].lvl);
        delay_ms(sound[i].duration);
    }

    pwm_set_ccr(&pwm_cfg, 0);

    while (1) {};

    return 0;
}
