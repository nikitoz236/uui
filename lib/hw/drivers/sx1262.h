#pragma once

#include <stdint.h>
#include "spi.h"
#include "gpio.h"

typedef struct {
    spi_dev_cfg_t spi_dev;
    const gpio_t * reset;
    const gpio_t * busy;
} sx1262_cfg_t;

/* SX1262 chip modes (bits [6:4] of status byte) */
enum {
    SX1262_MODE_UNUSED = 0,
    SX1262_MODE_RFU = 1,
    SX1262_MODE_STBY_RC = 2,
    SX1262_MODE_STBY_XOSC = 3,
    SX1262_MODE_FS = 4,
    SX1262_MODE_RX = 5,
    SX1262_MODE_TX = 6,
};

/* SX1262 command status (bits [3:1] of status byte) */
enum {
    SX1262_CMD_RFU = 0,
    SX1262_CMD_SUCCESS = 1,
    SX1262_CMD_DATA_AVAILABLE = 2,
    SX1262_CMD_TIMEOUT = 3,
    SX1262_CMD_ERROR = 4,
    SX1262_CMD_EXEC_FAIL = 5,
    SX1262_CMD_TX_DONE = 6,
};

/* StandbyConfig values */
#define SX1262_STDBY_RC 0x00
#define SX1262_STDBY_XOSC 0x01

/* PacketType values */
#define SX1262_PKT_TYPE_GFSK 0x00
#define SX1262_PKT_TYPE_LORA 0x01

/* LoRa spreading factor */
#define SX1262_SF7 0x07
#define SX1262_SF8 0x08
#define SX1262_SF9 0x09
#define SX1262_SF10 0x0A
#define SX1262_SF11 0x0B
#define SX1262_SF12 0x0C

/* LoRa bandwidth */
#define SX1262_BW_125 0x04
#define SX1262_BW_250 0x05
#define SX1262_BW_500 0x06

/* LoRa coding rate */
#define SX1262_CR_4_5 0x01
#define SX1262_CR_4_6 0x02
#define SX1262_CR_4_7 0x03
#define SX1262_CR_4_8 0x04

/* RampTime for SetTxParams */
#define SX1262_RAMP_200U 0x04

/* IRQ mask bits */
#define SX1262_IRQ_TX_DONE (1 << 0)
#define SX1262_IRQ_RX_DONE (1 << 1)
#define SX1262_IRQ_PREAMBLE_DETECTED (1 << 2)
#define SX1262_IRQ_SYNC_WORD_VALID (1 << 3)
#define SX1262_IRQ_HEADER_VALID (1 << 4)
#define SX1262_IRQ_HEADER_ERR (1 << 5)
#define SX1262_IRQ_CRC_ERR (1 << 6)
#define SX1262_IRQ_CAD_DONE (1 << 7)
#define SX1262_IRQ_CAD_DETECTED (1 << 8)
#define SX1262_IRQ_TIMEOUT (1 << 9)

/* SetRx timeout: 0 = single mode, 0xFFFFFF = continuous */
#define SX1262_RX_CONTINUOUS 0xFFFFFF

/* Calibrate param bits */
#define SX1262_CALIBRATE_ALL 0x7F

/* TCXO voltage values for SetTcxoMode */
#define SX1262_TCXO_CTRL_1_6V 0x00
#define SX1262_TCXO_CTRL_1_7V 0x01
#define SX1262_TCXO_CTRL_1_8V 0x02
#define SX1262_TCXO_CTRL_2_2V 0x03
#define SX1262_TCXO_CTRL_2_4V 0x04
#define SX1262_TCXO_CTRL_2_7V 0x05
#define SX1262_TCXO_CTRL_3_0V 0x06
#define SX1262_TCXO_CTRL_3_3V 0x07

void sx1262_init_pins(const sx1262_cfg_t * cfg);
void sx1262_reset(const sx1262_cfg_t * cfg);
void sx1262_wait_busy(const sx1262_cfg_t * cfg);

uint8_t sx1262_get_status(const sx1262_cfg_t * cfg);
uint8_t sx1262_read_reg(const sx1262_cfg_t * cfg, uint16_t addr);
void sx1262_write_reg(const sx1262_cfg_t * cfg, uint16_t addr, uint8_t val);

void sx1262_set_standby(const sx1262_cfg_t * cfg, uint8_t mode);
void sx1262_set_tcxo_mode(const sx1262_cfg_t * cfg, uint8_t voltage, uint32_t timeout);
void sx1262_calibrate(const sx1262_cfg_t * cfg, uint8_t calib_param);
void sx1262_set_buffer_base_addr(const sx1262_cfg_t * cfg, uint8_t tx_base, uint8_t rx_base);
uint16_t sx1262_get_device_errors(const sx1262_cfg_t * cfg);
void sx1262_clear_device_errors(const sx1262_cfg_t * cfg);
void sx1262_calibrate_image(const sx1262_cfg_t * cfg, uint8_t freq1, uint8_t freq2);
void sx1262_set_packet_type(const sx1262_cfg_t * cfg, uint8_t type);
void sx1262_set_rf_frequency(const sx1262_cfg_t * cfg, uint32_t freq_hz);
void sx1262_set_dio2_rf_switch_ctrl(const sx1262_cfg_t * cfg, uint8_t enable);
void sx1262_set_pa_config(const sx1262_cfg_t * cfg, uint8_t pa_duty_cycle, uint8_t hp_max, uint8_t device_sel, uint8_t pa_lut);
void sx1262_set_tx_params(const sx1262_cfg_t * cfg, int8_t power, uint8_t ramp_time);
void sx1262_set_mod_params(const sx1262_cfg_t * cfg, uint8_t sf, uint8_t bw, uint8_t cr, uint8_t ldro);
void sx1262_set_packet_params(const sx1262_cfg_t * cfg, uint16_t preamble_len, uint8_t header_type, uint8_t payload_len, uint8_t crc, uint8_t invert_iq);
void sx1262_set_dio_irq_params(const sx1262_cfg_t * cfg, uint16_t irq_mask, uint16_t dio1_mask, uint16_t dio2_mask, uint16_t dio3_mask);
uint16_t sx1262_get_irq_status(const sx1262_cfg_t * cfg);
void sx1262_clear_irq_status(const sx1262_cfg_t * cfg, uint16_t mask);
void sx1262_write_buffer(const sx1262_cfg_t * cfg, uint8_t offset, const uint8_t * data, uint8_t len);
void sx1262_read_buffer(const sx1262_cfg_t * cfg, uint8_t offset, uint8_t * data, uint8_t len);
void sx1262_get_rx_buffer_status(const sx1262_cfg_t * cfg, uint8_t * payload_len, uint8_t * rx_start_offset);
void sx1262_set_tx(const sx1262_cfg_t * cfg, uint32_t timeout);
void sx1262_set_rx(const sx1262_cfg_t * cfg, uint32_t timeout);

/* Returns 1 if chip responds correctly after reset, 0 otherwise */
int sx1262_check_connection(const sx1262_cfg_t * cfg);
