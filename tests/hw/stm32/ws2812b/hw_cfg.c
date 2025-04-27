#include "config.h"

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

// const ws2812_cfg_t ws2812_cfg = {
//     .pwm_cfg = &(pwm_cfg_t) {
//         .tim = TIM1,
//         .tim_pclk = HW_PCLK_TIM1,
//         .gpio = {
//             .port = GPIO_PORT_B,
//             .pin = 0
//         },
//         .ch = 2,
//         .max_val = 4,
//         .freq = 1000000,
//     }
// };
