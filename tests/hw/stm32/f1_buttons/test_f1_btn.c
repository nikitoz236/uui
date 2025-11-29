#include "periph_header.h"
#include "periph_rcc.h"
#include "periph_gpio.h"
#include "periph_pclk.h"
#include "stm_usart.h"
#include "systick.h"
#include "mstimer.h"
#include "time_units.h"

#define DP_NOTABLE
#include "dp.h"

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

const usart_cfg_t debug_usart = {
    .usart = USART1,
    .default_baud = 115200,
    .tx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 9
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
        }
    },
    .pclk = PCLK_USART1,
};

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&debug_usart, s, len);
}


#include "kbd.h"

uint8_t kbd_prev_state[1] = {};

kbd_cfg_t kbd = {
    .cols = &(const gpio_list_t){
        .count = 5,
        .cfg = {
            .mode = GPIO_MODE_INPUT,
            .pull = GPIO_PULL_NONE,
        },
        .pin_list = (gpio_pin_t []){
            { .port = GPIO_PORT_B, .pin = 3 },      // LU
            { .port = GPIO_PORT_A, .pin = 15 },     // LD
            { .port = GPIO_PORT_B, .pin = 4 },      // RU
            { .port = GPIO_PORT_B, .pin = 6 },      // RM
            { .port = GPIO_PORT_B, .pin = 7 },      // RD
        }
    },
    .prev_state = kbd_prev_state,
};

mstimer_t cyc_period = { .timeout = 100, .start = 0 };

void kbd_handler(unsigned num, unsigned state)
{
    dp("Key "); dpd(num, 2); dp(" = "); dpd(state, 1); dn();
}

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t)PCLK_AFIO, 1);

    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    pclk_ctrl(&(pclk_t)PCLK_IOPA, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPB, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPC, 1);

    init_systick();
    usart_set_cfg(&debug_usart);
    init_kbd(&kbd);

    __enable_irq();

    dpn("Hey bitch ! this is keyboard test!");

    while (1) {
        if (mstimer_do_period(&cyc_period)) {
            kbd_scan(kbd_handler);
        }
    };

    return 0;
}
