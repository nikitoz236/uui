#include "periph_header.h"
#include "periph_rcc.h"
#include "periph_pclk.h"
#include "periph_gpio.h"


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

void dc(char c)
{
    while(!(SPI2->SR & SPI_SR_TXE)) {};
    uint32_t x = c << 2;
    x |= 1 + (1 << 10);
    SPI2->DR = x;
}

void dpl(const void *x, unsigned l)
{
    const char * s = (const char *)x;
    while (l--) {
        dc(*s++);
    }
}









/*
    WS подключен к PB0
    TIM1_CH2N
    AF2
    DMA5 - TIM1_UP
*/

#include "periph_i2c.h"

i2c_cfg_t i2c1_cfg = {
    .pins = &(gpio_list_t){
        .pin_list = (gpio_pin_t []){
            { .port = GPIO_PORT_B, .pin = 6 },   // SCL
            { .port = GPIO_PORT_B, .pin = 7 },   // SDA
        },
        .count = 2,
        .cfg = {
            .mode = GPIO_MODE_AF,
            .type = GPIO_TYPE_OD,
            .pull = GPIO_PULL_NONE,
            .speed = GPIO_SPEED_HIGH,
            .af = 1,
        },
    },
    .i2c = I2C1,
    .irqn = I2C1_IRQn,
    .err_irqn = I2C1_ERR_IRQn,
    .freq = 100000,
    .pclk = PCLK_I2C1
};

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);
    pclk_ctrl(&(pclk_t)PCLK_GPIOA, 1);
    pclk_ctrl(&(pclk_t)PCLK_GPIOB, 1);
    pclk_ctrl(&(pclk_t)PCLK_DMA, 1);

    init_dbg_spi_usart();

    __enable_irq();

    init_i2c(&i2c1_cfg);

    for (unsigned i = 3; i < 127; i++) {
        dc(i);
        i2c_transaction(i, 0, 0, 0, 0);
        while (i2c_status() == I2C_STATUS_BUSY) {};
        dc(i2c_status());
    }

    while (1) {};

    return 0;
}
