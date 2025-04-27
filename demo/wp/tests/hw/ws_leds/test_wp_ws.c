#include "periph_header.h"
#include "periph_rcc.h"
#include "periph_pclk.h"
#include "periph_gpio.h"
#include "periph_spi.h"
#include "systick.h"
#include "stm_pwm.h"
#include "lcd_spi.h"


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

const gpio_t status_led = {
    .gpio = {
        .port = GPIO_PORT_A,
        .pin = 5
    },
    .cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_HIGH,
    }
};

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);
    pclk_ctrl(&(pclk_t)PCLK_GPIOA, 1);
    pclk_ctrl(&(pclk_t)PCLK_GPIOB, 1);

    init_gpio(&status_led);

    __enable_irq();

    gpio_set_state(&status_led, 1);

    while (1) {};

    return 0;
}
