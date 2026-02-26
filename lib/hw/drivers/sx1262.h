#pragma once

#include <stdint.h>
#include "spi.h"
#include "gpio.h"

/* ── пины SX1262 ──────────────────────────────────────────────────────── */

enum {
    SX1262_PIN_RESET,
    SX1262_PIN_BUSY,
    SX1262_PIN_DIO1,
    SX1262_PIN_COUNT,
};

/* ── конфигурация подключения ─────────────────────────────────────────── */

typedef struct {
    spi_dev_cfg_t spi_dev;
    const gpio_t * pin[SX1262_PIN_COUNT];
} sx1262_cfg_t;

/* ── таймаут RX ───────────────────────────────────────────────────────── */

#define SX1262_RX_CONTINUOUS 0xFFFFFF

/* ── управление ──────────────────────────────────────────────────────── */

void sx1262_init_pins(const sx1262_cfg_t * cfg);
void sx1262_reset(const sx1262_cfg_t * cfg);
void sx1262_wait_busy(const sx1262_cfg_t * cfg);

/* ── статус и регистры ────────────────────────────────────────────────── */

uint8_t sx1262_get_status(const sx1262_cfg_t * cfg);
uint8_t sx1262_read_reg(const sx1262_cfg_t * cfg, uint16_t addr);
void sx1262_write_reg(const sx1262_cfg_t * cfg, uint16_t addr, uint8_t val);
uint16_t sx1262_get_device_errors(const sx1262_cfg_t * cfg);
void sx1262_clear_device_errors(const sx1262_cfg_t * cfg);

/* ── инициализация радио ──────────────────────────────────────────────── */

void sx1262_set_standby(const sx1262_cfg_t * cfg, uint8_t mode);
void sx1262_set_tcxo_mode(const sx1262_cfg_t * cfg, uint8_t voltage, uint32_t timeout);
void sx1262_calibrate(const sx1262_cfg_t * cfg, uint8_t mask);
void sx1262_calibrate_image(const sx1262_cfg_t * cfg, uint8_t freq1, uint8_t freq2);
void sx1262_set_dio2_rf_switch_ctrl(const sx1262_cfg_t * cfg, unsigned enable);
void sx1262_set_packet_type(const sx1262_cfg_t * cfg);
void sx1262_set_rf_frequency(const sx1262_cfg_t * cfg, uint32_t freq_hz);

/* ── настройка канала ─────────────────────────────────────────────────── */

void sx1262_set_pa_config(const sx1262_cfg_t * cfg, uint8_t duty_cycle, uint8_t hp_max);
void sx1262_set_tx_params(const sx1262_cfg_t * cfg, int8_t power, uint8_t ramp_time);
void sx1262_set_mod_params(const sx1262_cfg_t * cfg, uint8_t sf, uint8_t bw, uint8_t cr, uint8_t ldro);
void sx1262_set_packet_params(const sx1262_cfg_t * cfg, uint16_t preamble_len, uint8_t header_type, uint8_t payload_len, uint8_t crc, uint8_t invert_iq);
void sx1262_set_buffer_base_addr(const sx1262_cfg_t * cfg, uint8_t tx_base, uint8_t rx_base);
void sx1262_set_irq_mask(const sx1262_cfg_t * cfg, uint16_t mask);

/* ── передача / приём ─────────────────────────────────────────────────── */

void sx1262_write_buffer(const sx1262_cfg_t * cfg, uint8_t offset, const uint8_t * data, uint8_t len);
void sx1262_read_buffer(const sx1262_cfg_t * cfg, uint8_t offset, uint8_t * data, uint8_t len);
void sx1262_set_tx(const sx1262_cfg_t * cfg, uint32_t timeout);
void sx1262_set_rx(const sx1262_cfg_t * cfg, uint32_t timeout);
uint16_t sx1262_get_irq_status(const sx1262_cfg_t * cfg);
void sx1262_clear_irq_status(const sx1262_cfg_t * cfg, uint16_t mask);
void sx1262_get_rx_buffer_status(const sx1262_cfg_t * cfg, uint8_t * payload_len, uint8_t * rx_start_offset);
void sx1262_get_packet_status(const sx1262_cfg_t * cfg, uint8_t * rssi_raw, int8_t * snr_raw);

/* ── проверка связи ───────────────────────────────────────────────────── */

unsigned sx1262_check_connection(const sx1262_cfg_t * cfg);
