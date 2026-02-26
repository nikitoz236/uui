#include "esp32_gpio.h"
#include "esp32_spi.h"
#include "esp32_i2c.h"
#include "xl9555.h"
#include "lora.h"

#include "dbg_usb_cdc_acm.h"

#define DP_NOTABLE
#include "dp.h"

#include "delay_blocking.h"

/*
 * T-LoRa Pager pin map:
 *
 * SPI bus:  MOSI=34 (FSPID), MISO=33 (FSPIQ), SCK=35 (FSPICLK)
 * SX1262:   CS=36, RESET=47, BUSY=48, DIO1=14
 * I2C bus:  SCL=2, SDA=3
 * XL9555:   addr=0x20, GPIO3 = LoRa power enable
 *
 * SX1262 на T-LoRa Pager имеет TCXO 32МГц питаемый от DIO3 (1.8В).
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

/* ── LoRa config ──────────────────────────────────────────────────────── */

#define PAYLOAD_LEN 16

static lora_cfg_t lora = {
    .chip = {
        .spi_dev = {
            .spi = &spi_bus,
            .cs_pin = &(gpio_t){
                .pin = { .pin = 36 },
                .cfg = { .mode = GPIO_MODE_OUT },
            },
        },
        .pin = {
            [SX1262_PIN_RESET] = &(gpio_t){
                .pin = { .pin = 47 },
                .cfg = { .mode = GPIO_MODE_OUT },
            },
            [SX1262_PIN_BUSY] = &(gpio_t){
                .pin = { .pin = 48 },
                .cfg = { .mode = GPIO_MODE_IN },
            },
            [SX1262_PIN_DIO1] = &(gpio_t){
                .pin = { .pin = 14 },
                .cfg = { .mode = GPIO_MODE_IN },
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
    sx1262_init_pins(&lora.chip);
    sx1262_reset(&lora.chip);

    if (!sx1262_check_connection(&lora.chip)) {
        dpn("sx1262 FAILED");
        while (1) {}
    }
    dpn("sx1262 ok");

    lora_init(&lora);
    dpn("lora ready, tx loop");

    uint8_t payload[PAYLOAD_LEN] = "hello from esp32";
    uint32_t pkt = 0;

    while (1) {
        pkt++;
        payload[0] = (uint8_t)pkt;

        if (lora_send(&lora, payload, PAYLOAD_LEN)) {
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
