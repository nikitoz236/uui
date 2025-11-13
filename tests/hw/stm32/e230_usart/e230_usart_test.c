#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "periph_gpio.h"

#include "systick.h"
#include "stm_usart.h"

#include "mstimer.h"
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

const usart_cfg_t debug_usart;

// void debug_usart_irq_handler(void)
// {
//     usart_irq_handler(&debug_usart);
// }

const usart_cfg_t debug_usart = {
    .usart = USART1,
    .default_baud = 115200,
    .rx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 10
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .pull = GPIO_PULL_NONE,
            .af = 1
        }
    },
    .tx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 9
            // .pin = 2
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
            .af = 1
        }
    },
    // .tx_dma = {
    //     .dma_ch = 2,
    // },
    .pclk = PCLK_USART1,
};

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&debug_usart, s, len);
}

gpio_t led_gpio = {
    .gpio = { .port = GPIO_PORT_B, .pin = 1 },
    .cfg = { .mode = GPIO_MODE_OUTPUT }
};

int main()
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t)PCLK_GPIOA, 1);
    pclk_ctrl(&(pclk_t)PCLK_GPIOB, 1);

    pclk_ctrl(&(pclk_t)PCLK_DMA, 1);

    init_gpio(&led_gpio);
    gpio_set_state(&led_gpio, 1);

    usart_set_cfg(&debug_usart);

    // init_systick();
    // __enable_irq();


    dpn("GD32E230 usart demo");

    // dn();
    // dp("SYS CLOCK: ");
    // dpd(pclk_f_hclk(), 10); dn();

    while (1) {};

    return 0;
}
