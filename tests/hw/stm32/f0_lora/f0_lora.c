#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "periph_gpio.h"
#include "periph_spi.h"

#include "systick.h"
#include "stm_usart.h"
#include "sx1262.h"

#define DP_NOTABLE
#include "dp.h"

/*
 * STM32F051 + SX1262 (HT-RA62) LoRa test
 *
 * SPI1:    SCK=PB3/AF0  MISO=PB4/AF0  MOSI=PB5/AF0
 * SX1262:  CS=PB6  RESET=PB7  BUSY=PB0  DIO1=PB1
 * USART1:  TX=PA9/AF1  RX=PA10/AF1
 */

/* -- debug USART --------------------------------------------------------- */

const usart_cfg_t debug_usart = {
    .usart = USART1,
    .default_baud = 115200,
    .tx_pin = &(const gpio_t){
        .gpio = { .port = GPIO_PORT_A, .pin = 9 },
        .cfg = { .mode = GPIO_MODE_AF, .speed = GPIO_SPEED_HIGH, .type = GPIO_TYPE_PP, .af = 1 },
    },
    .rx_pin = &(const gpio_t){
        .gpio = { .port = GPIO_PORT_A, .pin = 10 },
        .cfg = { .mode = GPIO_MODE_AF, .pull = GPIO_PULL_NONE, .af = 1 },
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

/* -- clocks --------------------------------------------------------------- */

const rcc_cfg_t rcc_cfg = {
    .hse_val = 8000000,
    .pll_src = PLL_SRC_PREDIV,
    .pll_prediv = 1,
    .pll_mul = 6,
    .sysclk_src = SYSCLK_SRC_PLL,
    .hclk_div = HCLK_DIV1,
    .apb_div = { APB_DIV1 },
};

/* -- SPI bus + SX1262 config ---------------------------------------------- */

static sx1262_cfg_t sx1262 = {
    .spi_dev = {
        .cs_pin = &(gpio_t){
            .gpio = { .port = GPIO_PORT_B, .pin = 6 },
            .cfg = { .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_HIGH, .type = GPIO_TYPE_PP },
        },
        .spi = &(spi_cfg_t){
            .spi = SPI1,
            .pclk = PCLK_SPI1,
            .clock_div = SPI_DIV_16,
            .pin_list = {
                [SPI_PIN_SCK] = &(gpio_t){
                    .gpio = { .port = GPIO_PORT_B, .pin = 3 },
                    .cfg = { .mode = GPIO_MODE_AF, .speed = GPIO_SPEED_HIGH, .type = GPIO_TYPE_PP, .af = 0 },
                },
                [SPI_PIN_MOSI] = &(gpio_t){
                    .gpio = { .port = GPIO_PORT_B, .pin = 5 },
                    .cfg = { .mode = GPIO_MODE_AF, .speed = GPIO_SPEED_HIGH, .type = GPIO_TYPE_PP, .af = 0 },
                },
                [SPI_PIN_MISO] = &(gpio_t){
                    .gpio = { .port = GPIO_PORT_B, .pin = 4 },
                    .cfg = { .mode = GPIO_MODE_AF, .speed = GPIO_SPEED_HIGH, .af = 0 },
                },
            },
        },
    },
    .reset = &(gpio_t){
        .gpio = { .port = GPIO_PORT_B, .pin = 7 },
        .cfg = { .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_LOW, .type = GPIO_TYPE_PP },
    },
    .busy = &(gpio_t){
        .gpio = { .port = GPIO_PORT_B, .pin = 0 },
        .cfg = { .mode = GPIO_MODE_INPUT },
    },
};

/* -- main ----------------------------------------------------------------- */

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t) PCLK_GPIOA, 1);
    pclk_ctrl(&(pclk_t) PCLK_GPIOB, 1);
    pclk_ctrl(&(pclk_t) PCLK_DMA, 1);

    usart_set_cfg(&debug_usart);

    init_systick();
    __enable_irq();

    dpn("f0_lora start");

    /* SPI init */
    init_spi(sx1262.spi_dev.spi);

    /* SX1262 pins (CS, RESET, BUSY) */
    sx1262_init_pins(&sx1262);

    /* hardware reset */
    dpn("sx1262 reset");
    sx1262_reset(&sx1262);

    /* check connection */
    uint8_t status = sx1262_get_status(&sx1262);
    dp("status: ");
    dpx(status, 1);
    dn();

    uint8_t chip_mode = (status >> 4) & 0x7;
    dp("chip mode: ");
    dpd(chip_mode, 1);
    if (chip_mode == SX1262_MODE_STBY_RC) {
        dpn(" (STBY_RC - ok)");
    } else if (chip_mode == 0) {
        dpn(" (no response)");
    } else {
        dpn(" (unexpected)");
    }

    sx1262_wait_busy(&sx1262);
    uint8_t rx_gain = sx1262_read_reg(&sx1262, 0x08AC);
    dp("RegRxGain: ");
    dpx(rx_gain, 1);
    if (rx_gain == 0x94) {
        dpn(" (default - ok)");
    } else {
        dpn(" (unexpected)");
    }

    if (sx1262_check_connection(&sx1262)) {
        dpn("sx1262 connected ok");
    } else {
        dpn("sx1262 connection FAILED");
    }

    dpn("done");
    while (1) {}

    return 0;
}
