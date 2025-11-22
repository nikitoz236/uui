#include "periph_header.h"
#include "periph_rcc.h"
#include "periph_pclk.h"
#include "periph_gpio.h"

#include "stm_ws2812b.h"

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

const gpio_list_t dbg_pins = {
    .count = 5,
    .pin_list = (gpio_pin_t []){
        { .port = GPIO_PORT_B, .pin = 3},
        { .port = GPIO_PORT_B, .pin = 5},
        { .port = GPIO_PORT_B, .pin = 4},
        { .port = GPIO_PORT_A, .pin = 12},
        { .port = GPIO_PORT_A, .pin = 15},
    },
    .cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_HIGH,
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











gpio_t dbg_spi_pin = {
    .gpio = { .port = GPIO_PORT_A, .pin = 14 },
    .cfg = { .mode = GPIO_MODE_AF, .af = 6 }
};

#define DBG_SPI SPI2

void init_dbg_spi_usart(void)
{
    pclk_ctrl(&(pclk_t)PCLK_SPI2, 1);
    init_gpio(&dbg_spi_pin);

    DBG_SPI->CR2 |= SPI_CR2_DS_0 * 0x0F;
    DBG_SPI->CR2 &= (~SPI_CR2_DS) | (SPI_CR2_DS_0 * (11 - 1));

    DBG_SPI->CR1 =
        SPI_CR1_MSTR +          // Master configuration
        SPI_CR1_SSM +           // Software slave management
        SPI_CR1_SSI +           // Internal slave select
        // SPI_CR2_SSOE +
        (SPI_CR1_BR_0 * 3) +
        SPI_CR1_LSBFIRST +
        0;

    DBG_SPI->CR1 |= SPI_CR1_SPE;
}

void wb_dc(char c)
{
    while(!(SPI2->SR & SPI_SR_TXE)) {};
    uint32_t x = c << 2;
    x |= 1 + (1 << 10);
    SPI2->DR = x;
}

void wb_dpl(const void *x, unsigned l)
{
    const char * s = (const char *)x;
    while (l--) {
        wb_dc(*s++);
    }
}









/*
    WS подключен к PB0
    TIM1_CH2N
    AF2
    DMA5 - TIM1_UP
*/

const ws_cfg_t ws_cfg = {
    .dma_ch = 5,
    .tim_ch = 2 - 1,
    .dma_irq = DMA1_Channel4_5_IRQn,
    .gpio = {
        .cfg = {
            .mode = GPIO_MODE_AF,
            .type = GPIO_TYPE_PP,
            .speed = GPIO_SPEED_HIGH,
            .af = 2
        },
        .gpio = {
            .port = GPIO_PORT_B,
            .pin = 0
        }
    },
    .tim = TIM1,
    .tim_pclk = PCLK_TIM1
};


ws_rgb_t ws_colors[16] = {

    { .r = 1, .g = 0, .b = 11 },
    { .r = 0, .g = 1, .b = 1 },
    { .r = 1, .g = 2, .b = 0 },
    { .r = 2, .g = 0, .b = 1 },
    { .r = 32, .g = 0, .b = 0 },
    { .r = 0, .g = 32, .b = 0 },
    { .r = 0, .g = 0, .b = 32 },
    { .r = 16, .g = 16, .b = 0 },
    { .r = 32, .g = 0, .b = 0 },
    { .r = 0, .g = 16, .b = 0 },
    { .r = 0, .g = 0, .b = 16 },
    { .r = 16, .g = 16, .b = 0 },
    { .r = 16, .g = 0, .b = 16 },
    { .r = 0, .g = 16, .b = 16 },
    { .r = 16, .g = 32, .b = 0 },
    { .r = 32, .g = 0, .b = 16 },

};

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);
    pclk_ctrl(&(pclk_t)PCLK_GPIOA, 1);
    pclk_ctrl(&(pclk_t)PCLK_GPIOB, 1);
    pclk_ctrl(&(pclk_t)PCLK_DMA, 1);

    init_dbg_spi_usart();

    __enable_irq();

    init_ws(&ws_cfg);

    // DBG_C(0x5E);
    for (volatile unsigned i = 0; i < 100000; i++) {};
    ws_update(ws_colors, 16);

    while (1) {};

    return 0;
}
