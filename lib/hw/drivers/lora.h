#pragma once

#include "sx1262.h"

/* ── параметры модуляции LoRa ─────────────────────────────────────────── */

typedef struct {
    sx1262_sf_t sf;
    sx1262_bw_t bw;
    sx1262_cr_t cr;
    sx1262_ldro_t ldro;
} lora_mod_params_t;

/* ── параметры пакета LoRa ────────────────────────────────────────────── */

typedef struct {
    uint16_t preamble_len;
    sx1262_header_t header_type;
    sx1262_crc_t crc;
    sx1262_iq_t invert_iq;
} lora_pkt_params_t;

/* ── конфигурация LoRa ────────────────────────────────────────────────── */

typedef struct {
    sx1262_cfg_t chip;
    uint32_t freq_hz;
    int8_t power; /* dBm: -9 .. +22 */
    unsigned tcxo : 1;
    sx1262_tcxo_voltage_t tcxo_voltage;
    unsigned dio2_rf_switch : 1;
    sx1262_irq_dio_t irq_dio : 2;
    lora_mod_params_t mod;
    lora_pkt_params_t pkt;
} lora_cfg_t;

/* ── операции ─────────────────────────────────────────────────────────── */

unsigned lora_init(const lora_cfg_t * cfg);
unsigned lora_send(const uint8_t * data, unsigned len);
void lora_rx_start(void);
unsigned lora_rx_check_dio_pin(void);
unsigned lora_rx_read(uint8_t * data, unsigned max_len);
void lora_standby(void);
void lora_get_packet_status(int8_t * rssi, int8_t * snr);

/* ── конвертация raw → человеческие единицы ───────────────────────────── */

/*
 * RSSI (Received Signal Strength Indicator).
 * SX1262 GetPacketStatus возвращает uint8_t (0..255).
 * Формула: RSSI [dBm] = -raw / 2.
 * Диапазон результата: 0 .. -127 dBm.
 * Типичные значения: -30 (сильный), -90 (слабый), -120 (порог).
 */
static inline int8_t lora_rssi_dbm(uint8_t rssi)
{
    return (int8_t)(-(int)rssi / 2);
}

/*
 * SNR (Signal-to-Noise Ratio).
 * SX1262 GetPacketStatus возвращает int8_t (-128..127).
 * Формула: SNR [dB] = raw / 4.
 * Диапазон результата: -32 .. +31 dB.
 * Положительный — сигнал выше шума, отрицательный — ниже
 * (LoRa декодирует до ~ -20 dB благодаря chirp spread spectrum).
 */
static inline int8_t lora_snr_db(int8_t snr)
{
    return snr / 4;
}
