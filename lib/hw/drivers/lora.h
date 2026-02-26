#pragma once

#include "sx1262.h"

/* ── параметры модуляции LoRa ─────────────────────────────────────────── */

typedef struct {
    enum {
        LORA_SF7  = 0x07,
        LORA_SF8  = 0x08,
        LORA_SF9  = 0x09,
        LORA_SF10 = 0x0A,
        LORA_SF11 = 0x0B,
        LORA_SF12 = 0x0C,
    } sf;
    enum {
        LORA_BW_125 = 0x04,
        LORA_BW_250 = 0x05,
        LORA_BW_500 = 0x06,
    } bw;
    enum {
        LORA_CR_4_5 = 0x01,
        LORA_CR_4_6 = 0x02,
        LORA_CR_4_7 = 0x03,
        LORA_CR_4_8 = 0x04,
    } cr;
    enum {
        LORA_LDRO_OFF = 0x00,
        LORA_LDRO_ON  = 0x01,
    } ldro;
} lora_mod_params_t;

/* ── параметры пакета LoRa ────────────────────────────────────────────── */

typedef struct {
    uint16_t preamble_len;
    enum {
        LORA_HEADER_EXPLICIT = 0x00,
        LORA_HEADER_IMPLICIT = 0x01,
    } header_type;
    enum {
        LORA_CRC_OFF = 0x00,
        LORA_CRC_ON  = 0x01,
    } crc;
    enum {
        LORA_IQ_STANDARD = 0x00,
        LORA_IQ_INVERTED = 0x01,
    } invert_iq;
} lora_pkt_params_t;

/* ── конфигурация LoRa ────────────────────────────────────────────────── */

typedef struct {
    sx1262_cfg_t chip;
    uint32_t freq_hz;
    int8_t power; /* dBm: -9 .. +22 */
    enum {
        LORA_TCXO_NONE = 0xFF,
        LORA_TCXO_1_6V = 0x00,
        LORA_TCXO_1_7V = 0x01,
        LORA_TCXO_1_8V = 0x02,
        LORA_TCXO_2_2V = 0x03,
        LORA_TCXO_2_4V = 0x04,
        LORA_TCXO_2_7V = 0x05,
        LORA_TCXO_3_0V = 0x06,
        LORA_TCXO_3_3V = 0x07,
    } tcxo_voltage;
    unsigned dio2_rf_switch : 1;
    lora_mod_params_t mod;
    lora_pkt_params_t pkt;
} lora_cfg_t;

/* ── флаги прерываний ─────────────────────────────────────────────────── */

typedef union {
    uint16_t raw;
    struct {
        unsigned tx_done : 1;  /* bit 0 */
        unsigned rx_done : 1;  /* bit 1 */
        unsigned : 4;          /* bits 2-5 */
        unsigned crc_err : 1;  /* bit 6 */
        unsigned : 2;          /* bits 7-8 */
        unsigned timeout : 1;  /* bit 9 */
        unsigned : 6;          /* bits 10-15 */
    };
} lora_irq_t;

/* ── операции ─────────────────────────────────────────────────────────── */

void lora_init(const lora_cfg_t * cfg);
unsigned lora_send(const lora_cfg_t * cfg, const uint8_t * data, uint8_t len);
void lora_rx_start(const lora_cfg_t * cfg);
int lora_rx_read(const lora_cfg_t * cfg, uint8_t * data, uint8_t max_len);
