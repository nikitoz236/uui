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
    enum {
        LORA_IRQ_DIO1 = 1,
        LORA_IRQ_DIO2 = 2,
        LORA_IRQ_DIO3 = 3,
    } irq_dio : 2;
    lora_mod_params_t mod;
    lora_pkt_params_t pkt;
} lora_cfg_t;

/* ── операции ─────────────────────────────────────────────────────────── */

void lora_init(const lora_cfg_t * cfg);
unsigned lora_send(const lora_cfg_t * cfg, const uint8_t * data, uint8_t len);
void lora_rx_start(const lora_cfg_t * cfg);
int lora_rx_read(const lora_cfg_t * cfg, uint8_t * data, uint8_t max_len);
