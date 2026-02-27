#include <stdint.h>
#include "sx1262.h"
#include "buf_endian.h"
#include "delay_blocking.h"

static inline void u24_to_be_buf8(uint8_t * buf, uint32_t value)
{
    buf[0] = (value >> 16) & 0xFF;
    buf[1] = (value >> 8) & 0xFF;
    buf[2] = value & 0xFF;
}

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

#define REG_RX_GAIN             0x08AC
#define REG_RX_GAIN_DEFAULT     0x94

/* ── хелпер SPI-транзакции ────────────────────────────────────────────── */

/*
 * Универсальная SPI-команда:
 *   wait_busy → CS low → opcode → tx[0..tx_len-1] → [skip status → rx[0..rx_len-1]] → CS high.
 *
 * rx_len == 0 — write-only.
 * rx_len > 0  — после tx пропускает статусный байт и читает ответ.
 */
static void sx1262_cmd(const sx1262_cfg_t * cfg, uint8_t opcode, const uint8_t * tx, unsigned tx_len, uint8_t * rx, unsigned rx_len)
{
    sx1262_wait_busy(cfg);
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, opcode);
    for (unsigned i = 0; i < tx_len; i++) {
        spi_write_8(cfg->spi_dev.spi, tx[i]);
    }
    if (rx_len) {
        sx1262_reg_status_t status = { .raw = spi_exchange_8(cfg->spi_dev.spi, 0x00) }; /* пока не используем */
        (void)status;
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

sx1262_reg_status_t sx1262_get_status(const sx1262_cfg_t * cfg)
{
    sx1262_reg_status_t status;
    sx1262_cmd(cfg, CMD_GET_STATUS, 0, 0, &status.raw, 1);
    return status;
}

uint8_t sx1262_read_reg(const sx1262_cfg_t * cfg, uint16_t addr)
{
    uint8_t tx[2];
    u16_to_be_buf8(&tx[0], addr);
    uint8_t val;
    sx1262_cmd(cfg, CMD_READ_REGISTER, &tx[0], sizeof(tx), &val, 1);
    return val;
}

void sx1262_write_reg(const sx1262_cfg_t * cfg, uint16_t addr, uint8_t val)
{
    struct {
        uint8_t addr[2];
        uint8_t val;
    } p = { .val = val };
    u16_to_be_buf8(&p.addr[0], addr);
    sx1262_cmd(cfg, CMD_WRITE_REGISTER, (uint8_t *)&p, sizeof(p), 0, 0);
}

sx1262_reg_errors_t sx1262_get_device_errors(const sx1262_cfg_t * cfg)
{
    uint8_t rx[2];
    sx1262_cmd(cfg, CMD_GET_DEVICE_ERRORS, 0, 0, &rx[0], 2);
    sx1262_reg_errors_t errors = { .raw = u16_from_be_buf8(&rx[0]) };
    return errors;
}

void sx1262_clear_device_errors(const sx1262_cfg_t * cfg)
{
    uint8_t p[2] = {};
    sx1262_cmd(cfg, CMD_CLEAR_DEVICE_ERRORS, &p[0], sizeof(p), 0, 0);
}

/* ── инициализация радио ──────────────────────────────────────────────── */

void sx1262_set_standby(const sx1262_cfg_t * cfg, sx1262_standby_t mode)
{
    uint8_t p = mode;
    sx1262_cmd(cfg, CMD_SET_STANDBY, &p, 1, 0, 0);
}

void sx1262_set_tcxo_mode(const sx1262_cfg_t * cfg, sx1262_tcxo_voltage_t voltage, uint32_t timeout_ms)
{
    struct __attribute__((packed)) {
        uint8_t voltage : 3;
        uint8_t : 5;
        uint8_t timeout[3];
    } p;
    u24_to_be_buf8(&p.timeout[0], timeout_ms * 64);
    p.voltage = voltage;
    sx1262_cmd(cfg, CMD_SET_TCXO_MODE, (uint8_t *)&p, sizeof(p), 0, 0);
}

void sx1262_calibrate(const sx1262_cfg_t * cfg, sx1262_reg_calib_t mask)
{
    sx1262_cmd(cfg, CMD_CALIBRATE, &mask.raw, 1, 0, 0);
}

void sx1262_calibrate_image(const sx1262_cfg_t * cfg, sx1262_calib_image_t img)
{
    sx1262_cmd(cfg, CMD_CALIBRATE_IMAGE, (uint8_t *)&img, sizeof(img), 0, 0);
}

void sx1262_set_dio2_rf_switch_ctrl(const sx1262_cfg_t * cfg, unsigned enable)
{
    uint8_t p = enable;
    sx1262_cmd(cfg, CMD_SET_DIO2_RF_SW, &p, 1, 0, 0);
}

void sx1262_set_packet_type(const sx1262_cfg_t * cfg, sx1262_packet_type_t type)
{
    uint8_t p = type;
    sx1262_cmd(cfg, CMD_SET_PACKET_TYPE, &p, 1, 0, 0);
}

void sx1262_set_rf_frequency(const sx1262_cfg_t * cfg, uint32_t freq_hz)
{
    uint32_t fkhz = freq_hz / 1000;
    uint32_t rf = fkhz * 1048 + fkhz * 576 / 1000;
    uint8_t p[4];
    u32_to_be_buf8(&p[0], rf);
    sx1262_cmd(cfg, CMD_SET_RF_FREQUENCY, &p[0], sizeof(p), 0, 0);
}

/* ── настройка канала ─────────────────────────────────────────────────── */

void sx1262_set_pa_config(const sx1262_cfg_t * cfg, sx1262_pa_t pa)
{
    static const struct {
        uint8_t duty_cycle;
        uint8_t hp_max;
    } table[] = {
        [SX1262_PA_22DBM] = { .duty_cycle = 0x04, .hp_max = 0x07 },
        [SX1262_PA_20DBM] = { .duty_cycle = 0x03, .hp_max = 0x05 },
        [SX1262_PA_17DBM] = { .duty_cycle = 0x02, .hp_max = 0x03 },
        [SX1262_PA_14DBM] = { .duty_cycle = 0x02, .hp_max = 0x02 },
    };
    struct {
        uint8_t duty_cycle;
        uint8_t hp_max;
        uint8_t device_sel;
        uint8_t pa_lut;
    } p = { .duty_cycle = table[pa].duty_cycle, .hp_max = table[pa].hp_max, .device_sel = 0x00, .pa_lut = 0x01 };
    sx1262_cmd(cfg, CMD_SET_PA_CONFIG, (uint8_t *)&p, sizeof(p), 0, 0);
}

void sx1262_set_tx_params(const sx1262_cfg_t * cfg, int8_t power, sx1262_ramp_t ramp_time)
{
    struct {
        uint8_t power;
        uint8_t ramp_time;
    } p = { .power = (uint8_t)power, .ramp_time = ramp_time };
    sx1262_cmd(cfg, CMD_SET_TX_PARAMS, (uint8_t *)&p, sizeof(p), 0, 0);
}

void sx1262_set_mod_params(const sx1262_cfg_t * cfg, sx1262_sf_t sf, sx1262_bw_t bw, sx1262_cr_t cr, sx1262_ldro_t ldro)
{
    struct {
        uint8_t sf;
        uint8_t bw;
        uint8_t cr;
        uint8_t ldro;
    } p = { .sf = sf, .bw = bw, .cr = cr, .ldro = ldro };
    sx1262_cmd(cfg, CMD_SET_MOD_PARAMS, (uint8_t *)&p, sizeof(p), 0, 0);
}

void sx1262_set_packet_params(const sx1262_cfg_t * cfg, uint16_t preamble, sx1262_header_t ht, uint8_t len, sx1262_crc_t crc, sx1262_iq_t iq)
{
    struct {
        uint8_t preamble[2];
        uint8_t ht;
        uint8_t len;
        uint8_t crc;
        uint8_t iq;
    } p = { .ht = ht, .len = len, .crc = crc, .iq = iq };
    u16_to_be_buf8(&p.preamble[0], preamble);
    sx1262_cmd(cfg, CMD_SET_PACKET_PARAMS, (uint8_t *)&p, sizeof(p), 0, 0);
}

void sx1262_set_buffer_base_addr(const sx1262_cfg_t * cfg, uint8_t tx_base, uint8_t rx_base)
{
    struct {
        uint8_t tx_base;
        uint8_t rx_base;
    } p = { .tx_base = tx_base, .rx_base = rx_base };
    sx1262_cmd(cfg, CMD_SET_BUF_BASE_ADDR, (uint8_t *)&p, sizeof(p), 0, 0);
}

void sx1262_set_irq_mask(const sx1262_cfg_t * cfg, sx1262_reg_irq_t mask, unsigned dio)
{
    sx1262_reg_irq_t p[4];
    u16_to_be_buf8((uint8_t *)&p[0], mask.raw);
    p[1].raw = 0;
    p[2].raw = 0;
    p[3].raw = 0;
    if (dio && dio <= 3) {
        p[dio] = p[0];
    }
    sx1262_cmd(cfg, CMD_SET_DIO_IRQ_PARAMS, (const uint8_t *)&p[0], sizeof(p), 0, 0);
}

/* ── передача / приём ─────────────────────────────────────────────────── */

void sx1262_write_buffer(const sx1262_cfg_t * cfg, uint8_t offset, const uint8_t * data, uint8_t len)
{
    /* WriteBuffer: opcode + offset + data — не ложится в sx1262_cmd() из-за переменной длины */
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
    sx1262_cmd(cfg, CMD_READ_BUFFER, &offset, 1, data, len);
}

void sx1262_set_tx(const sx1262_cfg_t * cfg, uint32_t timeout_ms)
{
    uint8_t p[3];
    u24_to_be_buf8(&p[0], timeout_ms * 64);
    sx1262_cmd(cfg, CMD_SET_TX, &p[0], sizeof(p), 0, 0);
}

void sx1262_set_rx(const sx1262_cfg_t * cfg, uint32_t timeout_ms)
{
    uint32_t ticks;
    if (timeout_ms == SX1262_RX_CONTINUOUS) {
        ticks = 0xFFFFFF;
    } else {
        ticks = timeout_ms * 64;
    }
    uint8_t p[3];
    u24_to_be_buf8(&p[0], ticks);
    sx1262_cmd(cfg, CMD_SET_RX, &p[0], sizeof(p), 0, 0);
}

sx1262_reg_irq_t sx1262_get_irq_status(const sx1262_cfg_t * cfg)
{
    uint8_t rx[2];
    sx1262_cmd(cfg, CMD_GET_IRQ_STATUS, 0, 0, &rx[0], 2);
    sx1262_reg_irq_t irq = { .raw = u16_from_be_buf8(&rx[0]) };
    return irq;
}

void sx1262_clear_irq_status(const sx1262_cfg_t * cfg, sx1262_reg_irq_t mask)
{
    uint8_t p[2];
    u16_to_be_buf8(&p[0], mask.raw);
    sx1262_cmd(cfg, CMD_CLEAR_IRQ_STATUS, &p[0], sizeof(p), 0, 0);
}

void sx1262_get_rx_buffer_status(const sx1262_cfg_t * cfg, uint8_t * payload_len, uint8_t * rx_start_offset)
{
    struct {
        uint8_t payload_len;
        uint8_t rx_offset;
    } rx;
    sx1262_cmd(cfg, CMD_GET_RX_BUF_STATUS, 0, 0, (uint8_t *)&rx, sizeof(rx));
    *payload_len = rx.payload_len;
    *rx_start_offset = rx.rx_offset;
}

void sx1262_get_packet_status(const sx1262_cfg_t * cfg, uint8_t * rssi_raw, int8_t * snr_raw)
{
    struct {
        uint8_t rssi;
        int8_t snr;
        uint8_t signal_rssi;
    } rx;
    sx1262_cmd(cfg, CMD_GET_PACKET_STATUS, 0, 0, (uint8_t *)&rx, sizeof(rx));
    *rssi_raw = rx.rssi;
    *snr_raw = rx.snr;
}

/* ── проверка связи ───────────────────────────────────────────────────── */

unsigned sx1262_check_connection(const sx1262_cfg_t * cfg)
{
    sx1262_reg_status_t status = sx1262_get_status(cfg);

    if (status.chip_mode != SX1262_MODE_STBY_RC) {
        return 0;
    }

    if (sx1262_read_reg(cfg, REG_RX_GAIN) != REG_RX_GAIN_DEFAULT) {
        return 0;
    }

    return 1;
}
