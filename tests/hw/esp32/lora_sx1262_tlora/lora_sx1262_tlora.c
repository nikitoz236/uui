#include "esp32_gpio.h"
#include "esp32_spi.h"
#include "esp32_i2c.h"
#include "xl9555.h"
#include "sx1262.h"

#include "dbg_usb_cdc_acm.h"

#define DP_NOTABLE
#include "dp.h"

#include "delay_blocking.h"

/*
 * T-LoRa Pager pin map:
 *
 * SPI bus:  MOSI=34 (FSPID), MISO=33 (FSPIQ), SCK=35 (FSPICLK)
 * SX1262:   CS=36, RESET=47, BUSY=48, IRQ=14
 * I2C bus:  SCL=2, SDA=3
 * XL9555:   addr=0x20, GPIO3 = LoRa power enable
 *
 * SX1262 на T-LoRa Pager имеет TCXO 32МГц питаемый от DIO3 (1.8В).
 * Инит: SetTcxoMode → Calibrate → настройка → TX loop.
 */

void __debug_usart_tx_data(const char * s, unsigned len)
{
    dbg_usb_cdc_acm_tx(s, len);
}

/* ── I2C bus ───────────────────────────────────────────────────────────── */

i2c_cfg_t i2c_bus_cfg = {
    .dev = &I2C0,
    .i2c_pclk = SYSTEM_I2C_EXT0_CLK_EN_S,
    .freq = 400000,
    .pin_list = &(gpio_list_t){
        .count = 2,
        .cfg = { .mode = GPIO_MODE_SIG_IO, .pu = 1, .od = 1 },
        .pin_list = {
            { .pin = 2, .signal = I2CEXT0_SCL_IN_IDX },
            { .pin = 3, .signal = I2CEXT0_SDA_IN_IDX } } }
};

/* ── XL9555 GPIO expander ──────────────────────────────────────────────── */

static const xl9555_gpio_t lora_pwr = {
    .addr_offset = 0,
    .pin = 3,
    .dir = XL9555_DIR_OUT,
    .polarity = XL9555_POL_NORMAL,
};

/* ── SPI bus ───────────────────────────────────────────────────────────── */

spi_cfg_t spi_bus = {
    .spi = &GPSPI2,
    .pin_list = {
        [SPI_PIN_SCK] = &(gpio_t){
            .cfg = { .mode = GPIO_MODE_SIG_OUT },
            .pin = { .pin = 35, .signal = FSPICLK_OUT_IDX },
        },
        [SPI_PIN_MOSI] = &(gpio_t){
            .cfg = { .mode = GPIO_MODE_SIG_OUT },
            .pin = { .pin = 34, .signal = FSPID_OUT_IDX },
        },
        [SPI_PIN_MISO] = &(gpio_t){
            .cfg = { .mode = GPIO_MODE_SIG_IN },
            .pin = { .pin = 33, .signal = FSPIQ_IN_IDX },
        },
    },
};

/* ── SX1262 ────────────────────────────────────────────────────────────── */

static sx1262_cfg_t sx1262 = {
    .spi_dev = {
        .spi = &spi_bus,
        .cs_pin = &(gpio_t){
            .pin = { .pin = 36 },
            .cfg = { .mode = GPIO_MODE_OUT },
        },
    },
    .reset = &(gpio_t){
        .pin = { .pin = 47 },
        .cfg = { .mode = GPIO_MODE_OUT },
    },
    .busy = &(gpio_t){
        .pin = { .pin = 48 },
        .cfg = { .mode = GPIO_MODE_IN },
    },
};

/* ── LoRa TX init ──────────────────────────────────────────────────────── */

#define PAYLOAD_LEN 16

static void lora_init(void)
{
    /* TCXO: DIO3 питает кристалл 1.8В, timeout ~5мс (320 тиков по 15.625мкс) */
    sx1262_set_tcxo_mode(&sx1262, SX1262_TCXO_CTRL_1_8V, 320);
    sx1262_wait_busy(&sx1262);

    /* Калибровка всех блоков после включения TCXO */
    sx1262_calibrate(&sx1262, SX1262_CALIBRATE_ALL);
    sx1262_wait_busy(&sx1262);

    /* Калибровка image rejection для 868 МГц (диапазон 863-870) */
    sx1262_calibrate_image(&sx1262, 0xD7, 0xDB);
    sx1262_wait_busy(&sx1262);

    /* DIO2 управляет RF switch */
    sx1262_set_dio2_rf_switch_ctrl(&sx1262, 1);
    sx1262_wait_busy(&sx1262);

    /* LoRa mode */
    sx1262_set_packet_type(&sx1262, SX1262_PKT_TYPE_LORA);
    sx1262_wait_busy(&sx1262);

    /* 868 МГц */
    sx1262_set_rf_frequency(&sx1262, 868000000);
    sx1262_wait_busy(&sx1262);

    /* PA config для SX1262 (не SX1261): duty=0x04, hp_max=0x07, device_sel=0x00, pa_lut=0x01 */
    sx1262_set_pa_config(&sx1262, 0x04, 0x07, 0x00, 0x01);
    sx1262_wait_busy(&sx1262);

    /* TX power: +14 dBm, ramp 200мкс */
    sx1262_set_tx_params(&sx1262, 14, SX1262_RAMP_200U);
    sx1262_wait_busy(&sx1262);

    /* Modulation: SF7, BW125, CR4/5, LDRO off */
    sx1262_set_mod_params(&sx1262, SX1262_SF7, SX1262_BW_125, SX1262_CR_4_5, 0);
    sx1262_wait_busy(&sx1262);

    /* Packet: preamble=8, explicit header, payload=PAYLOAD_LEN, CRC on, IQ normal */
    sx1262_set_packet_params(&sx1262, 8, 0x00, PAYLOAD_LEN, 0x01, 0x00);
    sx1262_wait_busy(&sx1262);

    /* Buffer base: TX=0, RX=128 */
    sx1262_set_buffer_base_addr(&sx1262, 0, 128);
    sx1262_wait_busy(&sx1262);

    /* IRQ: TX_DONE и TIMEOUT → DIO1 */
    sx1262_set_dio_irq_params(&sx1262,
        SX1262_IRQ_TX_DONE | SX1262_IRQ_TIMEOUT,
        SX1262_IRQ_TX_DONE | SX1262_IRQ_TIMEOUT,
        0, 0);
    sx1262_wait_busy(&sx1262);
}

/* ── main ──────────────────────────────────────────────────────────────── */

int main(void)
{
    dpn("lora tx start");

    init_i2c(&i2c_bus_cfg);

    dpn("lora power on");
    init_xl9555_gpio(&lora_pwr);
    xl9555_gpio_set(&lora_pwr, 1);
    delay_ms(20);

    init_spi(&spi_bus);
    sx1262_init_pins(&sx1262);
    sx1262_reset(&sx1262);

    if (!sx1262_check_connection(&sx1262)) {
        dpn("sx1262 FAILED");
        while (1) {}
    }
    dpn("sx1262 ok");

    lora_init();
    dpn("lora ready, tx loop");

    uint8_t payload[PAYLOAD_LEN] = "hello from esp32";
    uint32_t pkt = 0;

    while (1) {
        pkt++;
        payload[0] = (uint8_t)pkt;

        sx1262_clear_irq_status(&sx1262, 0xFFFF);
        sx1262_wait_busy(&sx1262);

        sx1262_write_buffer(&sx1262, 0, payload, PAYLOAD_LEN);
        sx1262_wait_busy(&sx1262);

        /* TX timeout: 3с = 3000мс * 64тика/мс = 192000 */
        sx1262_set_tx(&sx1262, 192000);

        uint16_t irq;
        do {
            sx1262_wait_busy(&sx1262);
            irq = sx1262_get_irq_status(&sx1262);
        } while (!(irq & (SX1262_IRQ_TX_DONE | SX1262_IRQ_TIMEOUT)));

        sx1262_clear_irq_status(&sx1262, irq);
        sx1262_wait_busy(&sx1262);

        if (irq & SX1262_IRQ_TX_DONE) {
            dp("tx ok  seq="); dpd(pkt, 5);
            dp("  len="); dpd(PAYLOAD_LEN, 2);
            dp("  data: "); dpxd(payload, 1, PAYLOAD_LEN);
            dn();
        } else {
            dp("tx err seq="); dpd(pkt, 5);
            dpn("  TIMEOUT");
        }

        delay_ms(1000);
    }
}
