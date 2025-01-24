#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"

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

const gpio_t led = {
    .gpio = {
        .port = GPIO_PORT_C,
        .pin = 13
    },
    .cfg = (gpio_cfg_t){
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_HIGH,
        .type = GPIO_TYPE_PP
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

    init_gpio(&led);
    gpio_set_state(&led, 1);

    while (1) {};

    return 0;
}
