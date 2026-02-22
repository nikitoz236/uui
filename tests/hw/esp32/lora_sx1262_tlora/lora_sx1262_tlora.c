#include "esp32_gpio.h"
#include "esp32_spi.h"
#include "esp32_i2c.h"

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

void __debug_usart_tx_data(const char *s, unsigned len)
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
            { .pin = 3, .signal = I2CEXT0_SDA_IN_IDX }
        }
    }
};

/* ── XL9555 GPIO expander ──────────────────────────────────────────────── */

enum xl9555_reg {
    XL9555_REG_INPUT    = 0,
    XL9555_REG_OUTPUT   = 1,
    XL9555_REG_POLARITY = 2,
    XL9555_REG_CONFIG   = 3,
};

#define XL9555_ADDR 0x20

static uint16_t xl9555_read(enum xl9555_reg reg)
{
    uint16_t val = 0;
    uint8_t cmd = reg << 1;
    i2c_transaction(XL9555_ADDR, &cmd, 1, &val, 2);
    while (i2c_status() == I2C_STATUS_BUSY) {}
    return val;
}

static void xl9555_write(enum xl9555_reg reg, uint16_t val)
{
    uint8_t data[3] = {
        [0] = reg << 1,
        [1] = val & 0xFF,
        [2] = (val >> 8) & 0xFF
    };
    i2c_transaction(XL9555_ADDR, data, 3, 0, 0);
    while (i2c_status() == I2C_STATUS_BUSY) {}
}

/* Enable LoRa power via XL9555 GPIO3 (output, active high) */
static void xl9555_lora_power_on(void)
{
    /* set GPIO3 as output (bit 3 = 0 in config reg) */
    uint16_t cfg = xl9555_read(XL9555_REG_CONFIG);
    cfg &= ~(1u << 3);
    xl9555_write(XL9555_REG_CONFIG, cfg);

    /* drive GPIO3 high */
    uint16_t out = xl9555_read(XL9555_REG_OUTPUT);
    out |= (1u << 3);
    xl9555_write(XL9555_REG_OUTPUT, out);
}

/* ── SPI bus ───────────────────────────────────────────────────────────── */

spi_cfg_t spi_bus = {
    .spi = &GPSPI2,
    .gpio_list = &(gpio_list_t){
        .count = 2,
        .cfg = { .mode = GPIO_MODE_SIG_OUT },
        .pin_list = {
            [SPI_PIN_MOSI] = { .pin = 34, .signal = FSPID_OUT_IDX },
            [SPI_PIN_SCK]  = { .pin = 35, .signal = FSPICLK_OUT_IDX }
        }
    }
};

static const gpio_t spi_miso = {
    .cfg = { .mode = GPIO_MODE_SIG_IN },
    .pin = { .pin = 33, .signal = FSPIQ_IN_IDX }
};

/* ── SX1262 control pins ───────────────────────────────────────────────── */

spi_dev_cfg_t sx1262_spi = {
    .spi = &spi_bus,
    .cs_pin = &(gpio_t){
        .pin = { .pin = 36 },
        .cfg = { .mode = GPIO_MODE_OUT }
    }
};

static const gpio_t sx1262_reset = {
    .pin = { .pin = 47 },
    .cfg = { .mode = GPIO_MODE_OUT }
};

static const gpio_t sx1262_busy = {
    .pin = { .pin = 48 },
    .cfg = { .mode = GPIO_MODE_IN }
};

/* ── SX1262 helpers ────────────────────────────────────────────────────── */

static void sx1262_wait_busy(void)
{
    while (gpio_get_state(&sx1262_busy)) {}
}

static void sx1262_reset_chip(void)
{
    gpio_set_state(&sx1262_reset, 0);
    delay_ms(2);
    gpio_set_state(&sx1262_reset, 1);
    delay_ms(10);
    sx1262_wait_busy();
}

/* GetStatus (0xC0): returns chip mode and command status */
static uint8_t sx1262_get_status(void)
{
    spi_dev_select(&sx1262_spi);
    spi_write_8(sx1262_spi.spi, 0xC0);
    uint8_t status = spi_exchange_8(sx1262_spi.spi, 0x00);
    spi_dev_unselect(&sx1262_spi);
    return status;
}

/* ReadRegister (0x1D): read one byte from 16-bit address */
static uint8_t sx1262_read_reg(uint16_t addr)
{
    spi_dev_select(&sx1262_spi);
    spi_write_8(sx1262_spi.spi, 0x1D);
    spi_write_8(sx1262_spi.spi, (addr >> 8) & 0xFF);
    spi_write_8(sx1262_spi.spi, addr & 0xFF);
    spi_exchange_8(sx1262_spi.spi, 0x00); /* status byte */
    uint8_t val = spi_exchange_8(sx1262_spi.spi, 0x00);
    spi_dev_unselect(&sx1262_spi);
    return val;
}

/* ── main ──────────────────────────────────────────────────────────────── */

int main(void)
{
    dpn("lora sx1262 tlora test");

    /* I2C init */
    init_i2c(&i2c_bus_cfg);

    /* Enable LoRa power via XL9555 GPIO3 */
    dpn("lora power on");
    xl9555_lora_power_on();
    delay_ms(20);

    /* SPI init */
    init_spi(&spi_bus);
    init_gpio(&spi_miso);
    init_spi_dev(&sx1262_spi);

    /* SX1262 control pins */
    init_gpio(&sx1262_reset);
    init_gpio(&sx1262_busy);

    /* Hardware reset */
    dpn("sx1262 reset");
    sx1262_reset_chip();

    /* Read status */
    uint8_t status = sx1262_get_status();
    dp("sx1262 status: "); dpx(status, 1); dn();

    /*
     * Expected after reset (STBY_RC mode):
     *   bits [6:4] = 010 (chip mode: STBY_RC)
     *   bits [3:1] = 001 (cmd status: success)
     *   => 0x22
     */
    uint8_t chip_mode = (status >> 4) & 0x7;
    dp("chip mode: "); dpd(chip_mode, 1);
    if (chip_mode == 2)
        dpn(" (STBY_RC - ok)");
    else if (chip_mode == 0)
        dpn(" (no response / not connected)");
    else
        dpn(" (unexpected)");

    /* Read RegRxGain (0x08AC) - default 0x94 */
    sx1262_wait_busy();
    uint8_t rx_gain = sx1262_read_reg(0x08AC);
    dp("RegRxGain (0x08AC): "); dpx(rx_gain, 1);
    if (rx_gain == 0x94)
        dpn(" (default - ok)");
    else
        dpn(" (unexpected)");

    dpn("done");
    while (1) {}
}
