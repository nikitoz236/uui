#include "esp32_gpio.h"
#include "esp32_spi.h"
#include "esp32_i2c.h"
#include "xl9555.h"
#include "lora.h"
#include "lora_ping_pong.h"

#include "dbg_usb_cdc_acm.h"

#define DP_NOTABLE
#include "dp.h"

#include "uptime.h"
#include "timers_32.h"
#include "delay_blocking.h"

/*
 * ESP32 T-LoRa Pager — ping сторона.
 *
 * Цикл: отправить ping (uint32_t seq) → переключиться в RX →
 * ждать pong с таймаутом → вывести статистику → пауза → повторить.
 *
 * SPI bus:  MOSI=34 MISO=33 SCK=35
 * SX1262:   CS=36 RESET=47 BUSY=48 DIO1=14
 * I2C bus:  SCL=2 SDA=3
 * XL9555:   addr=0x20, GPIO3 = LoRa power enable
 * TCXO 32МГц от DIO3 (1.8В), DIO2 — RF switch.
 */

#define PONG_TIMEOUT_MS 3000
#define PING_INTERVAL_MS 1000

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
    .tcxo = 1,
    .tcxo_voltage = SX1262_TCXO_1_8V,
    .dio2_rf_switch = 1,
    .mod = { .sf = SX1262_SF7, .bw = SX1262_BW_125, .cr = SX1262_CR_4_5, .ldro = SX1262_LDRO_OFF },
    .pkt = { .preamble_len = 8, .header_type = SX1262_HEADER_EXPLICIT, .crc = SX1262_CRC_ON, .invert_iq = SX1262_IQ_STANDARD },
};

ping_stats_t state = {};

int main(void)
{
    dpn("ping start");
    delay_ms(100);

    init_i2c(&i2c_bus_cfg);

    dpn("lora power on");
    init_xl9555_gpio(&lora_pwr);
    xl9555_gpio_set(&lora_pwr, 1);
    delay_ms(100);

    init_spi(&spi_bus);

    if (!lora_init(&lora)) {
        dpn("lora FAILED");
        while (1) {};
    }

    dpn("lora ready");

    while (1) {
        state.seq++;
        dp("sent: #"); dpd(state.seq, 6); dn();

        if (do_ping(&state, PONG_TIMEOUT_MS)) {

            dp("remote:   "); dp_lora_signal(&state.pong.q); dp("  seq: "); dpd(state.pong.seq, 6); dp("  cnt: "); dpd(state.pong.cnt, 6); dn();
            dp("received: "); dp_lora_signal(&state.q); dn();
        } else {
            dpn("timeout");
        }

        dn();
        delay_ms(PING_INTERVAL_MS);
    }
}
