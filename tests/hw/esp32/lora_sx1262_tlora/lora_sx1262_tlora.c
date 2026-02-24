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
 * T-LoRa Pager pin map (from hardware docs):
 *
 * SPI bus:  MOSI=34 (FSPID), MISO=33 (FSPIQ), SCK=35 (FSPICLK)
 * SX1262:   CS=36, RESET=47, BUSY=48, IRQ=14
 * I2C bus:  SCL=2, SDA=3
 * XL9555:   addr=0x20, GPIO3 = LoRa power enable
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

/* ── main ──────────────────────────────────────────────────────────────── */

int main(void)
{
    dpn("lora sx1262 tlora test");

    /* I2C init */
    init_i2c(&i2c_bus_cfg);

    /* Enable LoRa power via XL9555 GPIO3 */
    dpn("lora power on");
    init_xl9555_gpio(&lora_pwr);
    xl9555_gpio_set(&lora_pwr, 1);
    delay_ms(20);

    /* SPI init */
    init_spi(&spi_bus);
    sx1262_init_pins(&sx1262);

    /* Hardware reset */
    dpn("sx1262 reset");
    sx1262_reset(&sx1262);

    /* Read status */
    uint8_t status = sx1262_get_status(&sx1262);
    dp("sx1262 status: ");
    dpx(status, 1);
    dn();

    uint8_t chip_mode = (status >> 4) & 0x7;
    dp("chip mode: ");
    dpd(chip_mode, 1);
    if (chip_mode == SX1262_MODE_STBY_RC) {
        dpn(" (STBY_RC - ok)");
    } else if (chip_mode == 0) {
        dpn(" (no response / not connected)");
    } else {
        dpn(" (unexpected)");
    }

    /* Read RegRxGain (0x08AC) - default 0x94 */
    sx1262_wait_busy(&sx1262);
    uint8_t rx_gain = sx1262_read_reg(&sx1262, 0x08AC);
    dp("RegRxGain (0x08AC): ");
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
}
