#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "periph_gpio.h"
#include "periph_i2c.h"

#include "stm_usart.h"
#include "systick.h"

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

void debug_usart_irq_handler(void)
{
    usart_irq_handler(&debug_usart);
}

const usart_cfg_t debug_usart = {
    .usart = USART1,
    .default_baud = 115200,
    .rx_pin = &(const gpio_t){
        .gpio = { .port = GPIO_PORT_A, .pin = 10 },
        .cfg = { .mode = GPIO_MODE_AF, .pull = GPIO_PULL_NONE, .af = 1 },
    },
    .tx_pin = &(const gpio_t){
        .gpio = { .port = GPIO_PORT_A, .pin = 9 },
        .cfg = { .mode = GPIO_MODE_AF, .speed = GPIO_SPEED_HIGH, .type = GPIO_TYPE_PP, .af = 1 },
    },
    .tx_dma = {
        .dma_ch = 2,
    },
    .pclk = PCLK_USART1,
};

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&debug_usart, s, len);
}


const i2c_cfg_t i2c_bus = {
    .pins = &(gpio_list_t){
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_OD,
            .pull = GPIO_PULL_UP,
            .af = 1,
        },
        .count = 2,
        .pin_list = (gpio_pin_t[]){
            { .port = GPIO_PORT_B, .pin = 6 },
            { .port = GPIO_PORT_B, .pin = 7 },
        },
    },
    .i2c = I2C1,
    .irqn = I2C1_IRQn,
    .pclk = PCLK_I2C1,
    .clk_src = I2C_CLK_SYSCLK,
    I2C_TIMING_100K(48000000),
};


void i2c_scan(void)
{
    dpn("    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
    dp("00: ");

    for (unsigned i = 1; i < 128; i++) {
        if ((i & 0x0F) == 0) {
            dn();
            dpx(i, 1);
            dp(": ");
        }
        i2c_transaction(i, 0, 0, 0, 0);
        while (i2c_status() == I2C_STATUS_BUSY) {};
        if (i2c_status() == I2C_STATUS_NACK) {
            dp("-- ");
        } else {
            dpx(i, 1);
            dp(" ");
        }
    }
    dn();
}


int main(void)
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t)PCLK_GPIOA, 1);
    pclk_ctrl(&(pclk_t)PCLK_GPIOB, 1);
    pclk_ctrl(&(pclk_t)PCLK_DMA, 1);

    usart_set_cfg(&debug_usart);

    init_systick();
    __enable_irq();

    init_i2c(&i2c_bus);

    dpn("STM32F0 I2C scanner");
    i2c_scan();
    dpn("done");

    while (1) {};

    return 0;
}
