#include "esp32_gpio.h"
#include "esp32_spi.h"
#include "esp32_i2c.h"
#include "xl9555.h"
#include "st25r3916.h"

#include "dbg_usb_cdc_acm.h"

#define DP_NOTABLE
#include "dp.h"

#include "delay_blocking.h"

/*
 * T-LoRa Pager NFC reader test — ST25R3916.
 *
 * Pinout:
 *   SPI bus:  MOSI=34, MISO=33, SCK=35  (shared with LoRa/SD)
 *   NFC:     CS=39, IRQ=5
 *   I2C:     SCL=2, SDA=3
 *   XL9555:  addr=0x20, GPIO5 = NFC power enable
 *
 * Поток:
 *   1. I2C init → XL9555 → включить питание NFC
 *   2. SPI init → ST25R3916 init → проверить chip ID
 *   3. Включить RF field
 *   4. Цикл: poll NFC-A → если есть карта, вывести UID
 */

void __debug_usart_tx_data(const char * s, unsigned len)
{
    dbg_usb_cdc_acm_tx(s, len);
}

/* ── I2C bus ───────────────────────────────────────────────────── */

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

/* ── XL9555 GPIO expander — NFC power enable ───────────────────── */

static const xl9555_gpio_t nfc_pwr = {
    .addr_offset = 0,
    .pin = 5,                 /* GPIO5 на XL9555 = NFC_EN */
    .dir = XL9555_DIR_OUT,
    .polarity = XL9555_POL_NORMAL,
};

/* ── SPI bus ───────────────────────────────────────────────────── */

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

/* ── ST25R3916 NFC reader ──────────────────────────────────────── */

static st25r3916_cfg_t nfc_reader = {
    .spi_dev = {
        .spi = &spi_bus,
        .cs_pin = &(gpio_t){
            .pin = { .pin = 39 },
            .cfg = { .mode = GPIO_MODE_OUT },
        },
    },
    .irq = &(gpio_t){
        .pin = { .pin = 5 },
        .cfg = { .mode = GPIO_MODE_IN },
    },
};

/* ── main ──────────────────────────────────────────────────────── */

int main(void)
{
    dpn("");
    dpn("--- NFC reader test (ST25R3916) ---");
    dpn("");

    /* I2C */
    init_i2c(&i2c_bus_cfg);
    dpn("[init] i2c ok");

    /* NFC power cycle via XL9555 */
    init_xl9555_gpio(&nfc_pwr);
    xl9555_gpio_set(&nfc_pwr, 0);
    delay_ms(50);
    xl9555_gpio_set(&nfc_pwr, 1);
    delay_ms(100);
    dpn("[init] nfc power on");

    /* Поднять CS всех устройств на общей SPI шине чтобы не было
     * конфликта на MISO: SD(21), LoRa(36), LCD(38), NFC(39) */
    static const gpio_t cs_sd   = { .pin = { .pin = 21 }, .cfg = { .mode = GPIO_MODE_OUT } };
    static const gpio_t cs_lora = { .pin = { .pin = 36 }, .cfg = { .mode = GPIO_MODE_OUT } };
    static const gpio_t cs_lcd  = { .pin = { .pin = 38 }, .cfg = { .mode = GPIO_MODE_OUT } };
    static const gpio_t lora_rst = { .pin = { .pin = 47 }, .cfg = { .mode = GPIO_MODE_OUT } };

    init_gpio(&cs_sd);   gpio_set_state(&cs_sd, 1);
    init_gpio(&cs_lora); gpio_set_state(&cs_lora, 1);
    init_gpio(&cs_lcd);  gpio_set_state(&cs_lcd, 1);
    init_gpio(&lora_rst); gpio_set_state(&lora_rst, 1);
    dpn("[init] spi cs deselected (21,36,38,47)");

    /* SPI */
    init_spi(&spi_bus);
    dpn("[init] spi ok");

    /* ST25R3916 init */
    dpn("[nfc]  init...");
    if (!st_init(&nfc_reader)) {
        dpn("[nfc]  INIT FAILED");
        while (1) {}
    }

    /* RF field on */
    dpn("[nfc]  field on...");
    st_field_on(&nfc_reader);
    dpn("[nfc]  polling for NFC-A cards");
    dpn("");

    /* Polling loop */
    nfca_card_t card;
    uint32_t poll_count = 0;
    bool card_present = false;
    unsigned miss_count = 0;

    while (1) {
        poll_count++;

        if (st_nfca_poll(&nfc_reader, &card)) {
            miss_count = 0;
            if (!card_present) {
                card_present = true;
                dp("[card] UID=");
                for (unsigned i = 0; i < card.uid_len; i++) {
                    dpx(card.uid[i], 1);
                    if (i < card.uid_len - 1) dp(":");
                }
                dp("  ATQA=");
                dpx(card.atqa[0], 1); dpx(card.atqa[1], 1);
                dp("  SAK=");
                dpx(card.sak, 1);
                dn();
            }
            delay_ms(200);
        } else {
            miss_count++;
            if (card_present && miss_count > 5) {
                card_present = false;
                dpn("[card] removed");
            }
        }

        if ((poll_count % 500) == 0) {
            dp("."); /* heartbeat */
        }

        delay_ms(50);
    }
}
