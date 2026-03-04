#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "periph_gpio.h"
#include "periph_spi.h"

#include "systick.h"
#include "stm_usart.h"
#include "lora.h"

#define DP_NOTABLE
#include "dp.h"

#include "lora_ping_pong.h"

/*
 * STM32F051 + E220-900MM22S (LLCC68) — pong сторона.
 *
 * Цикл: ждём ping (uint32_t seq) → считаем потери →
 * собираем lora_pong_t с RSSI/SNR → отправляем → обратно в RX.
 *
 * SPI1:    SCK=PB3/AF0  MISO=PB4/AF0  MOSI=PB5/AF0
 * LLCC68:  CS=PB6  RESET=PB7  BUSY=PB0  DIO1=PB1
 * USART1:  TX=PA9/AF1  RX=PA10/AF1
 *
 * E220-900MM22S: LLCC68, crystal 32MHz (no TCXO),
 * TXEN/RXEN pins for RF switch (not DIO2).
 * DIO3 from LLCC68 connected to MCU PB1 (irq_dio=DIO3).
 *   868 MHz, SF7, BW125, CR4/5, preamble=8, explicit header, CRC on.
 */

/* -- debug USART ---------------------------------------------------------- */

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

/* -- LoRa config ---------------------------------------------------------- */

static lora_cfg_t lora = {
    .chip = {
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
        .pin = {
            [SX1262_PIN_RESET] = &(gpio_t){
                .gpio = { .port = GPIO_PORT_B, .pin = 7 },
                .cfg = { .mode = GPIO_MODE_OUTPUT, .speed = GPIO_SPEED_LOW, .type = GPIO_TYPE_PP },
            },
            [SX1262_PIN_BUSY] = &(gpio_t){
                .gpio = { .port = GPIO_PORT_B, .pin = 0 },
                .cfg = { .mode = GPIO_MODE_INPUT },
            },
            [SX1262_PIN_DIO1] = &(gpio_t){
                .gpio = { .port = GPIO_PORT_B, .pin = 1 },
                .cfg = { .mode = GPIO_MODE_INPUT },
            },
        },
    },
    .freq_hz = 868000000,
    .power = 14,
    .dio2_rf_switch = 0,               /* E220: TXEN/RXEN pins, not DIO2 */
    .irq_dio = SX1262_IRQ_DIO3,         /* E220: DIO3 connected to MCU, not DIO1 */
    .mod = { .sf = SX1262_SF7, .bw = SX1262_BW_125, .cr = SX1262_CR_4_5, .ldro = SX1262_LDRO_OFF },
    .pkt = { .preamble_len = 8, .header_type = SX1262_HEADER_EXPLICIT, .crc = SX1262_CRC_ON, .invert_iq = SX1262_IQ_STANDARD },
};

pong_state_t state = {};

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t) PCLK_GPIOA, 1);
    pclk_ctrl(&(pclk_t) PCLK_GPIOB, 1);
    pclk_ctrl(&(pclk_t) PCLK_DMA, 1);

    usart_set_cfg(&debug_usart);

    init_systick();
    __enable_irq();

    dpn("pong start (e220)");

    init_spi(lora.chip.spi_dev.spi);

    if (!lora_init(&lora)) {
        dpn("lora FAILED");
        while (1) {};
    }

    sx1262_reg_errors_t errors = sx1262_get_device_errors(&lora.chip);
    dp("device errors: "); dpx(errors.raw, 2); dn();

    dpn("waiting for pings...");

    while (1) {
        do_responce(&state);
    }
}
