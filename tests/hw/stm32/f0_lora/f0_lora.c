#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "periph_gpio.h"
#include "periph_spi.h"

#include "systick.h"
#include "stm_usart.h"
#include "lora.h"
#include "delay_blocking.h"

#define DP_NOTABLE
#include "dp.h"

/*
 * STM32F051 + SX1262 (HT-RA62) LoRa RX test
 *
 * SPI1:    SCK=PB3/AF0  MISO=PB4/AF0  MOSI=PB5/AF0
 * SX1262:  CS=PB6  RESET=PB7  BUSY=PB0  DIO1=PB1
 * USART1:  TX=PA9/AF1  RX=PA10/AF1
 *
 * HT-RA62: TCXO 32МГц питаемый от DIO3 (1.8В), DIO2 — RF switch.
 * Параметры должны совпадать с TX стороной (ESP32):
 *   868 МГц, SF7, BW125, CR4/5, preamble=8, explicit header, CRC on.
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
    .tcxo_voltage = LORA_TCXO_1_8V,
    .dio2_rf_switch = 1,
    .mod = { .sf = LORA_SF7, .bw = LORA_BW_125, .cr = LORA_CR_4_5, .ldro = LORA_LDRO_OFF },
    .pkt = { .preamble_len = 8, .header_type = LORA_HEADER_EXPLICIT, .crc = LORA_CRC_ON, .invert_iq = LORA_IQ_STANDARD },
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

    dpn("lora rx start");

    init_spi(lora.chip.spi_dev.spi);
    sx1262_init_pins(&lora.chip);
    sx1262_reset(&lora.chip);

    if (!sx1262_check_connection(&lora.chip)) {
        dpn("sx1262 FAILED");
        while (1) {}
    }
    dpn("sx1262 ok");

    lora_init(&lora);

    uint16_t errors = sx1262_get_device_errors(&lora.chip);
    dp("device errors: "); dpx(errors, 2); dn();

    uint8_t status = sx1262_get_status(&lora.chip);
    dp("status after init: "); dpx(status, 1); dn();

    dpn("lora ready, waiting...");

    lora_rx_start(&lora);

    status = sx1262_get_status(&lora.chip);
    dp("status in rx: "); dpx(status, 1); dn();

    uint32_t pkt = 0;

    while (1) {
        uint8_t buf[33];
        int n = lora_rx_read(&lora, buf, sizeof(buf) - 1);

        if (n > 0) {
            uint8_t rssi_raw;
            int8_t snr_raw;
            sx1262_get_packet_status(&lora.chip, &rssi_raw, &snr_raw);

            pkt++;
            dp("rx ok  seq="); dpd(buf[0], 5);
            dp("  rssi=-"); dpd(rssi_raw / 2, 3);
            dp("  snr=");
            if (snr_raw < 0) {
                dp("-"); dpd((unsigned)(-snr_raw) / 4, 2);
            } else {
                dpd((unsigned)snr_raw / 4, 3);
            }
            dp("  cnt="); dpd(pkt, 5);
            dn();
        } else if (n < 0) {
            dpn("rx crc error");
        }
    }
}
