#include <stdint.h>
#include "sx1262.h"
#include "delay_blocking.h"

/* ── опкоды SX1262 ────────────────────────────────────────────────────── */

#define CMD_GET_STATUS          0xC0
#define CMD_READ_REGISTER       0x1D
#define CMD_WRITE_REGISTER      0x0D
#define CMD_SET_STANDBY         0x80
#define CMD_SET_PACKET_TYPE     0x8A
#define CMD_SET_RF_FREQUENCY    0x86
#define CMD_SET_PA_CONFIG       0x95
#define CMD_SET_TX_PARAMS       0x8E
#define CMD_SET_MOD_PARAMS      0x8B
#define CMD_SET_PACKET_PARAMS   0x8C
#define CMD_SET_DIO2_RF_SW      0x9D
#define CMD_WRITE_BUFFER        0x0E
#define CMD_READ_BUFFER         0x1E
#define CMD_SET_TX              0x83
#define CMD_SET_RX              0x82
#define CMD_GET_IRQ_STATUS      0x12
#define CMD_CLEAR_IRQ_STATUS    0x02
#define CMD_SET_DIO_IRQ_PARAMS  0x08
#define CMD_GET_RX_BUF_STATUS   0x13
#define CMD_GET_PACKET_STATUS   0x14
#define CMD_SET_TCXO_MODE       0x97
#define CMD_CALIBRATE           0x89
#define CMD_SET_BUF_BASE_ADDR   0x8F
#define CMD_GET_DEVICE_ERRORS   0x17
#define CMD_CLEAR_DEVICE_ERRORS 0x07
#define CMD_CALIBRATE_IMAGE     0x98

/* ── внутренние константы ─────────────────────────────────────────────── */

#define REG_RX_GAIN         0x08AC
#define REG_RX_GAIN_DEFAULT 0x94

enum {
    MODE_STBY_RC   = 2,
    MODE_STBY_XOSC = 3,
    MODE_FS        = 4,
    MODE_RX        = 5,
    MODE_TX        = 6,
};

/* ── хелпер SPI-транзакции ────────────────────────────────────────────── */

/*
 * Универсальная SPI-команда:
 *   wait_busy → CS low → opcode → tx[0..tx_len-1] → [skip status → rx[0..rx_len-1]] → CS high.
 *
 * rx_len == 0 — write-only.
 * rx_len > 0  — после tx пропускает статусный байт и читает ответ.
 */
static void cmd(const sx1262_cfg_t * cfg, uint8_t opcode, const uint8_t * tx, unsigned tx_len, uint8_t * rx, unsigned rx_len)
{
    sx1262_wait_busy(cfg);
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, opcode);
    for (unsigned i = 0; i < tx_len; i++) {
        spi_write_8(cfg->spi_dev.spi, tx[i]);
    }
    if (rx_len) {
        spi_exchange_8(cfg->spi_dev.spi, 0x00); /* статусный байт — пропускаем */
        for (unsigned i = 0; i < rx_len; i++) {
            rx[i] = spi_exchange_8(cfg->spi_dev.spi, 0x00);
        }
    }
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── управление ──────────────────────────────────────────────────────── */

void sx1262_init_pins(const sx1262_cfg_t * cfg)
{
    init_spi_dev(&cfg->spi_dev);
    for (unsigned i = 0; i < SX1262_PIN_COUNT; i++) {
        init_gpio(cfg->pin[i]);
    }
}

void sx1262_wait_busy(const sx1262_cfg_t * cfg)
{
    while (gpio_get_state(cfg->pin[SX1262_PIN_BUSY])) {};
}

void sx1262_reset(const sx1262_cfg_t * cfg)
{
    gpio_set_state(cfg->pin[SX1262_PIN_RESET], 0);
    delay_ms(2);
    gpio_set_state(cfg->pin[SX1262_PIN_RESET], 1);
    delay_ms(10);
    sx1262_wait_busy(cfg);
}

/* ── статус и регистры ────────────────────────────────────────────────── */

uint8_t sx1262_get_status(const sx1262_cfg_t * cfg)
{
    /* GetStatus — особый случай: ответ сразу после опкода, без skip status */
    sx1262_wait_busy(cfg);
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, CMD_GET_STATUS);
    uint8_t status = spi_exchange_8(cfg->spi_dev.spi, 0x00);
    spi_dev_unselect(&cfg->spi_dev);
    return status;
}

uint8_t sx1262_read_reg(const sx1262_cfg_t * cfg, uint16_t addr)
{
    uint8_t tx[] = { (addr >> 8) & 0xFF, addr & 0xFF };
    uint8_t val;
    cmd(cfg, CMD_READ_REGISTER, tx, sizeof(tx), &val, 1);
    return val;
}

void sx1262_write_reg(const sx1262_cfg_t * cfg, uint16_t addr, uint8_t val)
{
    uint8_t p[] = { (addr >> 8) & 0xFF, addr & 0xFF, val };
    cmd(cfg, CMD_WRITE_REGISTER, p, sizeof(p), 0, 0);
}

uint16_t sx1262_get_device_errors(const sx1262_cfg_t * cfg)
{
    uint8_t rx[2];
    cmd(cfg, CMD_GET_DEVICE_ERRORS, 0, 0, rx, 2);
    return ((uint16_t)rx[0] << 8) | rx[1];
}

void sx1262_clear_device_errors(const sx1262_cfg_t * cfg)
{
    uint8_t p[] = { 0x00, 0x00 };
    cmd(cfg, CMD_CLEAR_DEVICE_ERRORS, p, sizeof(p), 0, 0);
}

/* ── инициализация радио ──────────────────────────────────────────────── */

void sx1262_set_standby(const sx1262_cfg_t * cfg, uint8_t mode)
{
    cmd(cfg, CMD_SET_STANDBY, &mode, 1, 0, 0);
}

void sx1262_set_tcxo_mode(const sx1262_cfg_t * cfg, uint8_t voltage, uint32_t timeout)
{
    uint8_t p[] = { voltage & 0x07, (timeout >> 16) & 0xFF, (timeout >> 8) & 0xFF, timeout & 0xFF };
    cmd(cfg, CMD_SET_TCXO_MODE, p, sizeof(p), 0, 0);
}

void sx1262_calibrate(const sx1262_cfg_t * cfg, uint8_t mask)
{
    cmd(cfg, CMD_CALIBRATE, &mask, 1, 0, 0);
}

void sx1262_calibrate_image(const sx1262_cfg_t * cfg, uint8_t freq1, uint8_t freq2)
{
    uint8_t p[] = { freq1, freq2 };
    cmd(cfg, CMD_CALIBRATE_IMAGE, p, sizeof(p), 0, 0);
}

void sx1262_set_dio2_rf_switch_ctrl(const sx1262_cfg_t * cfg, unsigned enable)
{
    uint8_t p = enable;
    cmd(cfg, CMD_SET_DIO2_RF_SW, &p, 1, 0, 0);
}

void sx1262_set_packet_type(const sx1262_cfg_t * cfg)
{
    uint8_t type = 0x01; /* LoRa */
    cmd(cfg, CMD_SET_PACKET_TYPE, &type, 1, 0, 0);
}

void sx1262_set_rf_frequency(const sx1262_cfg_t * cfg, uint32_t freq_hz)
{
    uint32_t fkhz = freq_hz / 1000;
    uint32_t rf = fkhz * 1048 + fkhz * 576 / 1000;
    uint8_t p[] = { (rf >> 24) & 0xFF, (rf >> 16) & 0xFF, (rf >> 8) & 0xFF, rf & 0xFF };
    cmd(cfg, CMD_SET_RF_FREQUENCY, p, sizeof(p), 0, 0);
}

/* ── настройка канала ─────────────────────────────────────────────────── */

void sx1262_set_pa_config(const sx1262_cfg_t * cfg, uint8_t duty_cycle, uint8_t hp_max)
{
    uint8_t p[] = { duty_cycle, hp_max, 0x00, 0x01 }; /* device_sel=SX1262, pa_lut=1 */
    cmd(cfg, CMD_SET_PA_CONFIG, p, sizeof(p), 0, 0);
}

void sx1262_set_tx_params(const sx1262_cfg_t * cfg, int8_t power, uint8_t ramp_time)
{
    uint8_t p[] = { (uint8_t)power, ramp_time };
    cmd(cfg, CMD_SET_TX_PARAMS, p, sizeof(p), 0, 0);
}

void sx1262_set_mod_params(const sx1262_cfg_t * cfg, uint8_t sf, uint8_t bw, uint8_t cr, uint8_t ldro)
{
    uint8_t p[] = { sf, bw, cr, ldro };
    cmd(cfg, CMD_SET_MOD_PARAMS, p, sizeof(p), 0, 0);
}

void sx1262_set_packet_params(const sx1262_cfg_t * cfg, uint16_t preamble_len, uint8_t header_type, uint8_t payload_len, uint8_t crc, uint8_t invert_iq)
{
    uint8_t p[] = { (preamble_len >> 8) & 0xFF, preamble_len & 0xFF, header_type, payload_len, crc, invert_iq };
    cmd(cfg, CMD_SET_PACKET_PARAMS, p, sizeof(p), 0, 0);
}

void sx1262_set_buffer_base_addr(const sx1262_cfg_t * cfg, uint8_t tx_base, uint8_t rx_base)
{
    uint8_t p[] = { tx_base, rx_base };
    cmd(cfg, CMD_SET_BUF_BASE_ADDR, p, sizeof(p), 0, 0);
}

void sx1262_set_irq_mask(const sx1262_cfg_t * cfg, uint16_t mask)
{
    uint8_t p[] = { (mask >> 8) & 0xFF, mask & 0xFF, (mask >> 8) & 0xFF, mask & 0xFF, 0x00, 0x00, 0x00, 0x00 };
    cmd(cfg, CMD_SET_DIO_IRQ_PARAMS, p, sizeof(p), 0, 0);
}

/* ── передача / приём ─────────────────────────────────────────────────── */

void sx1262_write_buffer(const sx1262_cfg_t * cfg, uint8_t offset, const uint8_t * data, uint8_t len)
{
    /* WriteBuffer: opcode + offset + data — не ложится в cmd() из-за offset */
    sx1262_wait_busy(cfg);
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, CMD_WRITE_BUFFER);
    spi_write_8(cfg->spi_dev.spi, offset);
    for (unsigned i = 0; i < len; i++) {
        spi_write_8(cfg->spi_dev.spi, data[i]);
    }
    spi_dev_unselect(&cfg->spi_dev);
}

void sx1262_read_buffer(const sx1262_cfg_t * cfg, uint8_t offset, uint8_t * data, uint8_t len)
{
    uint8_t tx[] = { offset };
    cmd(cfg, CMD_READ_BUFFER, tx, sizeof(tx), data, len);
}

void sx1262_set_tx(const sx1262_cfg_t * cfg, uint32_t timeout)
{
    uint8_t p[] = { (timeout >> 16) & 0xFF, (timeout >> 8) & 0xFF, timeout & 0xFF };
    cmd(cfg, CMD_SET_TX, p, sizeof(p), 0, 0);
}

void sx1262_set_rx(const sx1262_cfg_t * cfg, uint32_t timeout)
{
    uint8_t p[] = { (timeout >> 16) & 0xFF, (timeout >> 8) & 0xFF, timeout & 0xFF };
    cmd(cfg, CMD_SET_RX, p, sizeof(p), 0, 0);
}

uint16_t sx1262_get_irq_status(const sx1262_cfg_t * cfg)
{
    uint8_t rx[2];
    cmd(cfg, CMD_GET_IRQ_STATUS, 0, 0, rx, 2);
    return ((uint16_t)rx[0] << 8) | rx[1];
}

void sx1262_clear_irq_status(const sx1262_cfg_t * cfg, uint16_t mask)
{
    uint8_t p[] = { (mask >> 8) & 0xFF, mask & 0xFF };
    cmd(cfg, CMD_CLEAR_IRQ_STATUS, p, sizeof(p), 0, 0);
}

void sx1262_get_rx_buffer_status(const sx1262_cfg_t * cfg, uint8_t * payload_len, uint8_t * rx_start_offset)
{
    uint8_t rx[2];
    cmd(cfg, CMD_GET_RX_BUF_STATUS, 0, 0, rx, 2);
    *payload_len = rx[0];
    *rx_start_offset = rx[1];
}

void sx1262_get_packet_status(const sx1262_cfg_t * cfg, uint8_t * rssi_raw, int8_t * snr_raw)
{
    uint8_t rx[3];
    cmd(cfg, CMD_GET_PACKET_STATUS, 0, 0, rx, 3);
    *rssi_raw = rx[0];
    *snr_raw = (int8_t)rx[1];
}

/* ── проверка связи ───────────────────────────────────────────────────── */

unsigned sx1262_check_connection(const sx1262_cfg_t * cfg)
{
    uint8_t status = sx1262_get_status(cfg);
    unsigned chip_mode = (status >> 4) & 0x7;

    if (chip_mode != MODE_STBY_RC) {
        return 0;
    }

    if (sx1262_read_reg(cfg, REG_RX_GAIN) != REG_RX_GAIN_DEFAULT) {
        return 0;
    }

    return 1;
}
