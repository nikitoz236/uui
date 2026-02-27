#include "esp32_gpio.h"
#include "esp32_spi.h"
#include "esp32_i2c.h"
#include "xl9555.h"
#include "lora.h"
#include "lora_ping_pong.h"

#include "dbg_usb_cdc_acm.h"

#define DP_NOTABLE
#include "dp.h"

#include "esp32_systime.h"
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

/* ── debug print helpers ──────────────────────────────────────────────── */

/* печатает rssi/snr в компактном формате: -20/11 или -20/-3 */
static void print_rssi_snr(uint8_t rssi, int8_t snr)
{
    dp("-"); dpd(rssi / 2, 3);
    dp("/");
    if (snr < 0) {
        dp("-"); dpd((unsigned)(-snr) / 4, 2);
    } else {
        dpd((unsigned)snr / 4, 2);
    }
}

/* ── main ──────────────────────────────────────────────────────────────── */

int main(void)
{
    dpn("ping start");

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
    dpn("lora ready");

    uint32_t seq = 0;
    uint32_t pong_cnt = 0;
    uint32_t pong_lost = 0;

    while (1) {
        seq++;

        /* ── TX ping ──────────────────────────────────────────────── */

        if (!lora_send(&lora, (const uint8_t *)&seq, sizeof(seq))) {
            dp("TX ERR seq="); dpd(seq, 5); dpn(" TIMEOUT");
            delay_ms(PING_INTERVAL_MS);
            continue;
        }

        dp("ping #"); dpd(seq, 5); dp("  tx: ");
        dpxd(&seq, 1, sizeof(seq));
        dn();

        /* ── RX pong с таймаутом ──────────────────────────────────── */

        lora_rx_start(&lora);

        timer_32_t tim = {};
        t32_run(&tim, systimer_ms(0), PONG_TIMEOUT_MS);

        lora_pong_t pong;
        int got_pong = 0;

        while (t32_is_active(&tim, systimer_ms(0))) {
            int n = lora_rx_read(&lora, (uint8_t *)&pong, sizeof(pong));
            if (n == sizeof(pong)) {
                got_pong = 1;
                break;
            }
            if (n < 0) {
                dpn("pong crc err");
                break;
            }
        }

        /* ── остановить RX ────────────────────────────────────────── */

        sx1262_set_standby(&lora.chip, SX1262_STANDBY_RC);

        /* ── вывод ────────────────────────────────────────────────── */

        if (got_pong) {
            pong_cnt++;

            uint8_t rssi;
            int8_t snr;
            sx1262_get_packet_status(&lora.chip, &rssi, &snr);

            dp("pong #"); dpd(pong.rx_seq, 5); dp("  rx: ");
            dpxd(&pong, 1, sizeof(pong));
            dn();

            dp("  seq="); dpd(pong.rx_seq, 5);
            dp(" lost="); dpd(pong.rx_lost, 3);
            dp(" rx="); dpd(pong.rx_cnt, 5);
            dp(" tx="); dpd(pong.tx_cnt, 5);
            dp("  rem:"); print_rssi_snr(pong.rssi, pong.snr);
            dp("  loc:"); print_rssi_snr(rssi, snr);
            dp("  miss="); dpd(pong_lost, 5);
            dn();
        } else {
            pong_lost++;
            dp("TIMEOUT seq="); dpd(seq, 5);
            dp("  miss="); dpd(pong_lost, 5);
            dn();
        }

        delay_ms(PING_INTERVAL_MS);
    }
}
