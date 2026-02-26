#include <stdint.h>
#include "sx1262.h"
#include "delay_blocking.h"

/* ── SX1262 opcodes ───────────────────────────────────────────────────── */

#define SX1262_CMD_GET_STATUS 0xC0
#define SX1262_CMD_READ_REGISTER 0x1D
#define SX1262_CMD_WRITE_REGISTER 0x0D
#define SX1262_CMD_SET_STANDBY 0x80
#define SX1262_CMD_SET_PACKET_TYPE 0x8A
#define SX1262_CMD_SET_RF_FREQUENCY 0x86
#define SX1262_CMD_SET_PA_CONFIG 0x95
#define SX1262_CMD_SET_TX_PARAMS 0x8E
#define SX1262_CMD_SET_MOD_PARAMS 0x8B
#define SX1262_CMD_SET_PACKET_PARAMS 0x8C
#define SX1262_CMD_SET_DIO2_RF_SW_CTRL 0x9D
#define SX1262_CMD_WRITE_BUFFER 0x0E
#define SX1262_CMD_READ_BUFFER 0x1E
#define SX1262_CMD_SET_TX 0x83
#define SX1262_CMD_SET_RX 0x82
#define SX1262_CMD_GET_IRQ_STATUS 0x12
#define SX1262_CMD_CLEAR_IRQ_STATUS 0x02
#define SX1262_CMD_SET_DIO_IRQ_PARAMS 0x08
#define SX1262_CMD_GET_RX_BUFFER_STATUS 0x13
#define SX1262_CMD_SET_TCXO_MODE 0x97
#define SX1262_CMD_CALIBRATE 0x89
#define SX1262_CMD_SET_BUFFER_BASE_ADDR 0x8F
#define SX1262_CMD_GET_DEVICE_ERRORS 0x17
#define SX1262_CMD_CLEAR_DEVICE_ERRORS 0x07
#define SX1262_CMD_CALIBRATE_IMAGE 0x98

/* Known register for connection check */
#define SX1262_REG_RX_GAIN 0x08AC
#define SX1262_REG_RX_GAIN_DEFAULT 0x94

/* ── pin init ─────────────────────────────────────────────────────────── */

void sx1262_init_pins(const sx1262_cfg_t * cfg)
{
    init_spi_dev(&cfg->spi_dev);
    init_gpio(cfg->reset);
    init_gpio(cfg->busy);
}

/* ── busy wait ────────────────────────────────────────────────────────── */

void sx1262_wait_busy(const sx1262_cfg_t * cfg)
{
    while (gpio_get_state(cfg->busy)) {}
}

/* ── hardware reset ───────────────────────────────────────────────────── */

void sx1262_reset(const sx1262_cfg_t * cfg)
{
    gpio_set_state(cfg->reset, 0);
    delay_ms(2);
    gpio_set_state(cfg->reset, 1);
    delay_ms(10);
    sx1262_wait_busy(cfg);
}

/* ── GetStatus (0xC0) ─────────────────────────────────────────────────── */

uint8_t sx1262_get_status(const sx1262_cfg_t * cfg)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_GET_STATUS);
    uint8_t status = spi_exchange_8(cfg->spi_dev.spi, 0x00);
    spi_dev_unselect(&cfg->spi_dev);
    return status;
}

/* ── ReadRegister (0x1D) ──────────────────────────────────────────────── */

uint8_t sx1262_read_reg(const sx1262_cfg_t * cfg, uint16_t addr)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_READ_REGISTER);
    spi_write_8(cfg->spi_dev.spi, (addr >> 8) & 0xFF);
    spi_write_8(cfg->spi_dev.spi, addr & 0xFF);
    spi_exchange_8(cfg->spi_dev.spi, 0x00); /* status byte */
    uint8_t val = spi_exchange_8(cfg->spi_dev.spi, 0x00);
    spi_dev_unselect(&cfg->spi_dev);
    return val;
}

/* ── WriteRegister (0x0D) ─────────────────────────────────────────────── */

void sx1262_write_reg(const sx1262_cfg_t * cfg, uint16_t addr, uint8_t val)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_WRITE_REGISTER);
    spi_write_8(cfg->spi_dev.spi, (addr >> 8) & 0xFF);
    spi_write_8(cfg->spi_dev.spi, addr & 0xFF);
    spi_write_8(cfg->spi_dev.spi, val);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── SetStandby (0x80) ────────────────────────────────────────────────── */

void sx1262_set_standby(const sx1262_cfg_t * cfg, uint8_t mode)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_SET_STANDBY);
    spi_write_8(cfg->spi_dev.spi, mode);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── SetTcxoMode (0x97) ───────────────────────────────────────────────── */

void sx1262_set_tcxo_mode(const sx1262_cfg_t * cfg, uint8_t voltage, uint32_t timeout)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_SET_TCXO_MODE);
    spi_write_8(cfg->spi_dev.spi, voltage & 0x07);
    spi_write_8(cfg->spi_dev.spi, (timeout >> 16) & 0xFF);
    spi_write_8(cfg->spi_dev.spi, (timeout >> 8) & 0xFF);
    spi_write_8(cfg->spi_dev.spi, timeout & 0xFF);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── Calibrate (0x89) ────────────────────────────────────────────────── */

void sx1262_calibrate(const sx1262_cfg_t * cfg, uint8_t calib_param)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_CALIBRATE);
    spi_write_8(cfg->spi_dev.spi, calib_param);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── SetBufferBaseAddress (0x8F) ──────────────────────────────────────── */

void sx1262_set_buffer_base_addr(const sx1262_cfg_t * cfg, uint8_t tx_base, uint8_t rx_base)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_SET_BUFFER_BASE_ADDR);
    spi_write_8(cfg->spi_dev.spi, tx_base);
    spi_write_8(cfg->spi_dev.spi, rx_base);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── GetDeviceErrors (0x17) ───────────────────────────────────────────── */

uint16_t sx1262_get_device_errors(const sx1262_cfg_t * cfg)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_GET_DEVICE_ERRORS);
    spi_exchange_8(cfg->spi_dev.spi, 0x00); /* status */
    uint16_t err = (uint16_t)spi_exchange_8(cfg->spi_dev.spi, 0x00) << 8;
    err |= spi_exchange_8(cfg->spi_dev.spi, 0x00);
    spi_dev_unselect(&cfg->spi_dev);
    return err;
}

/* ── ClearDeviceErrors (0x07) ─────────────────────────────────────────── */

void sx1262_clear_device_errors(const sx1262_cfg_t * cfg)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_CLEAR_DEVICE_ERRORS);
    spi_write_8(cfg->spi_dev.spi, 0x00);
    spi_write_8(cfg->spi_dev.spi, 0x00);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── CalibrateImage (0x98) ────────────────────────────────────────────── */

void sx1262_calibrate_image(const sx1262_cfg_t * cfg, uint8_t freq1, uint8_t freq2)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_CALIBRATE_IMAGE);
    spi_write_8(cfg->spi_dev.spi, freq1);
    spi_write_8(cfg->spi_dev.spi, freq2);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── SetPacketType (0x8A) ─────────────────────────────────────────────── */

void sx1262_set_packet_type(const sx1262_cfg_t * cfg, uint8_t type)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_SET_PACKET_TYPE);
    spi_write_8(cfg->spi_dev.spi, type);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── SetRfFrequency (0x86) ────────────────────────────────────────────── */

void sx1262_set_rf_frequency(const sx1262_cfg_t * cfg, uint32_t freq_hz)
{
    /* rfFreq = freq_hz * 2^25 / 32e6 = freq_hz * 1048576 / 1000000
     * = freq_hz / 1000 * 1048 + freq_hz / 1000 * 576 / 1000
     * all ops fit in 32-bit: max freq ~1GHz -> freq/1000 ~1e6, *1048 ~1e9 < 2^32 */
    uint32_t fkhz = freq_hz / 1000;
    uint32_t rf_freq = fkhz * 1048 + fkhz * 576 / 1000;
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_SET_RF_FREQUENCY);
    spi_write_8(cfg->spi_dev.spi, (rf_freq >> 24) & 0xFF);
    spi_write_8(cfg->spi_dev.spi, (rf_freq >> 16) & 0xFF);
    spi_write_8(cfg->spi_dev.spi, (rf_freq >> 8) & 0xFF);
    spi_write_8(cfg->spi_dev.spi, rf_freq & 0xFF);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── SetDIO2AsRfSwitchCtrl (0x9D) ─────────────────────────────────────── */

void sx1262_set_dio2_rf_switch_ctrl(const sx1262_cfg_t * cfg, uint8_t enable)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_SET_DIO2_RF_SW_CTRL);
    spi_write_8(cfg->spi_dev.spi, enable);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── SetPaConfig (0x95) ───────────────────────────────────────────────── */

void sx1262_set_pa_config(const sx1262_cfg_t * cfg, uint8_t pa_duty_cycle, uint8_t hp_max, uint8_t device_sel, uint8_t pa_lut)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_SET_PA_CONFIG);
    spi_write_8(cfg->spi_dev.spi, pa_duty_cycle);
    spi_write_8(cfg->spi_dev.spi, hp_max);
    spi_write_8(cfg->spi_dev.spi, device_sel);
    spi_write_8(cfg->spi_dev.spi, pa_lut);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── SetTxParams (0x8E) ───────────────────────────────────────────────── */

void sx1262_set_tx_params(const sx1262_cfg_t * cfg, int8_t power, uint8_t ramp_time)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_SET_TX_PARAMS);
    spi_write_8(cfg->spi_dev.spi, (uint8_t)power);
    spi_write_8(cfg->spi_dev.spi, ramp_time);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── SetModulationParams (0x8B) ───────────────────────────────────────── */

void sx1262_set_mod_params(const sx1262_cfg_t * cfg, uint8_t sf, uint8_t bw, uint8_t cr, uint8_t ldro)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_SET_MOD_PARAMS);
    spi_write_8(cfg->spi_dev.spi, sf);
    spi_write_8(cfg->spi_dev.spi, bw);
    spi_write_8(cfg->spi_dev.spi, cr);
    spi_write_8(cfg->spi_dev.spi, ldro);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── SetPacketParams (0x8C) ───────────────────────────────────────────── */

void sx1262_set_packet_params(const sx1262_cfg_t * cfg, uint16_t preamble_len, uint8_t header_type, uint8_t payload_len, uint8_t crc, uint8_t invert_iq)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_SET_PACKET_PARAMS);
    spi_write_8(cfg->spi_dev.spi, (preamble_len >> 8) & 0xFF);
    spi_write_8(cfg->spi_dev.spi, preamble_len & 0xFF);
    spi_write_8(cfg->spi_dev.spi, header_type);
    spi_write_8(cfg->spi_dev.spi, payload_len);
    spi_write_8(cfg->spi_dev.spi, crc);
    spi_write_8(cfg->spi_dev.spi, invert_iq);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── SetDioIrqParams (0x08) ───────────────────────────────────────────── */

void sx1262_set_dio_irq_params(const sx1262_cfg_t * cfg, uint16_t irq_mask, uint16_t dio1_mask, uint16_t dio2_mask, uint16_t dio3_mask)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_SET_DIO_IRQ_PARAMS);
    spi_write_8(cfg->spi_dev.spi, (irq_mask >> 8) & 0xFF);
    spi_write_8(cfg->spi_dev.spi, irq_mask & 0xFF);
    spi_write_8(cfg->spi_dev.spi, (dio1_mask >> 8) & 0xFF);
    spi_write_8(cfg->spi_dev.spi, dio1_mask & 0xFF);
    spi_write_8(cfg->spi_dev.spi, (dio2_mask >> 8) & 0xFF);
    spi_write_8(cfg->spi_dev.spi, dio2_mask & 0xFF);
    spi_write_8(cfg->spi_dev.spi, (dio3_mask >> 8) & 0xFF);
    spi_write_8(cfg->spi_dev.spi, dio3_mask & 0xFF);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── GetIrqStatus (0x12) ──────────────────────────────────────────────── */

uint16_t sx1262_get_irq_status(const sx1262_cfg_t * cfg)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_GET_IRQ_STATUS);
    spi_exchange_8(cfg->spi_dev.spi, 0x00); /* status */
    uint16_t irq = (uint16_t)spi_exchange_8(cfg->spi_dev.spi, 0x00) << 8;
    irq |= spi_exchange_8(cfg->spi_dev.spi, 0x00);
    spi_dev_unselect(&cfg->spi_dev);
    return irq;
}

/* ── ClearIrqStatus (0x02) ────────────────────────────────────────────── */

void sx1262_clear_irq_status(const sx1262_cfg_t * cfg, uint16_t mask)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_CLEAR_IRQ_STATUS);
    spi_write_8(cfg->spi_dev.spi, (mask >> 8) & 0xFF);
    spi_write_8(cfg->spi_dev.spi, mask & 0xFF);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── WriteBuffer (0x0E) ───────────────────────────────────────────────── */

void sx1262_write_buffer(const sx1262_cfg_t * cfg, uint8_t offset, const uint8_t * data, uint8_t len)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_WRITE_BUFFER);
    spi_write_8(cfg->spi_dev.spi, offset);
    for (uint8_t i = 0; i < len; i++) {
        spi_write_8(cfg->spi_dev.spi, data[i]);
    }
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── ReadBuffer (0x1E) ────────────────────────────────────────────────── */

void sx1262_read_buffer(const sx1262_cfg_t * cfg, uint8_t offset, uint8_t * data, uint8_t len)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_READ_BUFFER);
    spi_write_8(cfg->spi_dev.spi, offset);
    spi_exchange_8(cfg->spi_dev.spi, 0x00); /* status */
    for (uint8_t i = 0; i < len; i++) {
        data[i] = spi_exchange_8(cfg->spi_dev.spi, 0x00);
    }
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── GetRxBufferStatus (0x13) ─────────────────────────────────────────── */

void sx1262_get_rx_buffer_status(const sx1262_cfg_t * cfg, uint8_t * payload_len, uint8_t * rx_start_offset)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_GET_RX_BUFFER_STATUS);
    spi_exchange_8(cfg->spi_dev.spi, 0x00); /* status */
    *payload_len = spi_exchange_8(cfg->spi_dev.spi, 0x00);
    *rx_start_offset = spi_exchange_8(cfg->spi_dev.spi, 0x00);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── SetTx (0x83) ─────────────────────────────────────────────────────── */

void sx1262_set_tx(const sx1262_cfg_t * cfg, uint32_t timeout)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_SET_TX);
    spi_write_8(cfg->spi_dev.spi, (timeout >> 16) & 0xFF);
    spi_write_8(cfg->spi_dev.spi, (timeout >> 8) & 0xFF);
    spi_write_8(cfg->spi_dev.spi, timeout & 0xFF);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── SetRx (0x82) ─────────────────────────────────────────────────────── */

void sx1262_set_rx(const sx1262_cfg_t * cfg, uint32_t timeout)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SX1262_CMD_SET_RX);
    spi_write_8(cfg->spi_dev.spi, (timeout >> 16) & 0xFF);
    spi_write_8(cfg->spi_dev.spi, (timeout >> 8) & 0xFF);
    spi_write_8(cfg->spi_dev.spi, timeout & 0xFF);
    spi_dev_unselect(&cfg->spi_dev);
}

/* ── connection check ─────────────────────────────────────────────────── */

/*
 * Verifies chip is alive by:
 *   1. GetStatus — chip mode should be STBY_RC (0x2x) after reset
 *   2. ReadRegister RegRxGain (0x08AC) — default 0x94
 *
 * Returns 1 if both checks pass, 0 otherwise.
 */
int sx1262_check_connection(const sx1262_cfg_t * cfg)
{
    uint8_t status = sx1262_get_status(cfg);
    uint8_t chip_mode = (status >> 4) & 0x7;

    if (chip_mode != SX1262_MODE_STBY_RC) {
        return 0;
    }

    sx1262_wait_busy(cfg);
    uint8_t rx_gain = sx1262_read_reg(cfg, SX1262_REG_RX_GAIN);

    if (rx_gain != SX1262_REG_RX_GAIN_DEFAULT) {
        return 0;
    }

    return 1;
}
