#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "periph_gpio.h"
#include "periph_spi.h"

#include "systick.h"
#include "stm_usart.h"
#include "sx1262.h"
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

/* -- LoRa RX init --------------------------------------------------------- */

static void lora_init(void)
{
    /* HT-RA62 имеет TCXO 32МГц. Включаем через DIO3, напряжение 1.8В.
     * timeout ~5мс (320 тиков по 15.625мкс) */
    sx1262_set_tcxo_mode(&sx1262, SX1262_TCXO_CTRL_1_8V, 320);
    sx1262_wait_busy(&sx1262);

    /* Калибровка всех блоков после включения TCXO */
    sx1262_calibrate(&sx1262, SX1262_CALIBRATE_ALL);
    sx1262_wait_busy(&sx1262);

    /* DIO2 управляет RF switch */
    sx1262_set_dio2_rf_switch_ctrl(&sx1262, 1);
    sx1262_wait_busy(&sx1262);

    /* Калибровка image rejection для 868 МГц (863-870) */
    sx1262_calibrate_image(&sx1262, 0xD7, 0xDB);
    sx1262_wait_busy(&sx1262);

    /* LoRa mode */
    sx1262_set_packet_type(&sx1262, SX1262_PKT_TYPE_LORA);
    sx1262_wait_busy(&sx1262);

    /* 868 МГц */
    sx1262_set_rf_frequency(&sx1262, 868000000);
    sx1262_wait_busy(&sx1262);

    /* Modulation: SF7, BW125, CR4/5, LDRO off — должно совпадать с TX */
    sx1262_set_mod_params(&sx1262, SX1262_SF7, SX1262_BW_125, SX1262_CR_4_5, 0);
    sx1262_wait_busy(&sx1262);

    /* Packet: preamble=8, explicit header, payload=16, CRC on, IQ normal */
    sx1262_set_packet_params(&sx1262, 8, 0x00, 16, 0x01, 0x00);
    sx1262_wait_busy(&sx1262);

    /* Buffer base: TX=0, RX=0 */
    sx1262_set_buffer_base_addr(&sx1262, 0, 0);
    sx1262_wait_busy(&sx1262);

    /* IRQ: RX_DONE, CRC_ERR, TIMEOUT → DIO1 */
    sx1262_set_dio_irq_params(&sx1262,
        SX1262_IRQ_RX_DONE | SX1262_IRQ_CRC_ERR | SX1262_IRQ_TIMEOUT,
        SX1262_IRQ_RX_DONE | SX1262_IRQ_CRC_ERR | SX1262_IRQ_TIMEOUT,
        0, 0);
    sx1262_wait_busy(&sx1262);
}

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

    init_spi(sx1262.spi_dev.spi);
    sx1262_init_pins(&sx1262);
    sx1262_reset(&sx1262);

    if (!sx1262_check_connection(&sx1262)) {
        dpn("sx1262 FAILED");
        while (1) {}
    }
    dpn("sx1262 ok");

    lora_init();

    uint16_t errors = sx1262_get_device_errors(&sx1262);
    sx1262_wait_busy(&sx1262);
    dp("device errors: ");
    dpx(errors, 2);
    dn();

    uint8_t status = sx1262_get_status(&sx1262);
    dp("status after init: ");
    dpx(status, 1);
    dn();

    dpn("lora ready, waiting...");

    sx1262_clear_irq_status(&sx1262, 0xFFFF);
    sx1262_wait_busy(&sx1262);

    /* Непрерывный приём */
    sx1262_set_rx(&sx1262, SX1262_RX_CONTINUOUS);
    sx1262_wait_busy(&sx1262);

    status = sx1262_get_status(&sx1262);
    dp("status in rx: ");
    dpx(status, 1);
    dn();

    uint32_t pkt = 0;

    while (1) {
        /* Polling IRQ без прерываний.
         * В RX_CONTINUOUS BUSY может быть высоким — не ждём его здесь. */
        uint16_t irq = sx1262_get_irq_status(&sx1262);

        if (irq & SX1262_IRQ_RX_DONE) {
            sx1262_clear_irq_status(&sx1262, irq);
            sx1262_wait_busy(&sx1262);

            if (irq & SX1262_IRQ_CRC_ERR) {
                dpn("rx crc error");
                continue;
            }

            uint8_t payload_len, rx_offset;
            sx1262_get_rx_buffer_status(&sx1262, &payload_len, &rx_offset);
            sx1262_wait_busy(&sx1262);

            uint8_t buf[33];
            if (payload_len > sizeof(buf) - 1) payload_len = sizeof(buf) - 1;
            sx1262_read_buffer(&sx1262, rx_offset, buf, payload_len);
            sx1262_wait_busy(&sx1262);

            pkt++;
            dp("rx ok  seq="); dpd(buf[0], 5);
            dp("  len="); dpd(payload_len, 2);
            dp("  cnt="); dpd(pkt, 5);
            dp("  data: "); dpxd(buf, 1, payload_len);
            dn();

        } else if (irq & SX1262_IRQ_TIMEOUT) {
            sx1262_clear_irq_status(&sx1262, irq);
            /* В режиме RX_CONTINUOUS timeout не должен срабатывать */
            dpn("rx timeout (unexpected)");
        }
    }
}
