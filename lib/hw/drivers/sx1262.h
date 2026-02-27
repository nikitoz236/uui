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

/* ── регистр прерываний ──────────────────────────────────────────────── */

typedef union {
    uint16_t raw;
    struct {
        uint8_t tx_done            : 1; /* bit 0 */
        uint8_t rx_done            : 1; /* bit 1 */
        uint8_t preamble_detected  : 1; /* bit 2 */
        uint8_t sync_word_valid    : 1; /* bit 3 */
        uint8_t header_valid       : 1; /* bit 4 */
        uint8_t header_err         : 1; /* bit 5 */
        uint8_t crc_err            : 1; /* bit 6 */
        uint8_t cad_done           : 1; /* bit 7 */
        uint8_t cad_detected       : 1; /* bit 8 */
        uint8_t timeout            : 1; /* bit 9 */
        uint8_t : 6;                    /* bits 10-15 reserved */
    };
} sx1262_reg_irq_t;

/* ── регистр статуса ──────────────────────────────────────────────────── */

typedef union {
    uint8_t raw;
    struct __attribute__((packed)) {
        uint8_t : 1;
        enum {
            SX1262_CMD_DATA_AVAILABLE   = 2,
            SX1262_CMD_TIMEOUT          = 3,
            SX1262_CMD_PROCESS_ERROR    = 4,
            SX1262_CMD_EXEC_FAILURE     = 5,
            SX1262_CMD_TX_DONE          = 6,
        } cmd_status    : 3;
        enum {
            SX1262_MODE_STBY_RC         = 2,
            SX1262_MODE_STBY_XOSC       = 3,
            SX1262_MODE_FS              = 4,
            SX1262_MODE_RX              = 5,
            SX1262_MODE_TX              = 6,
        } chip_mode     : 3;
        uint8_t : 1;
    };
} sx1262_reg_status_t;

/* ── регистр ошибок ──────────────────────────────────────────────────── */

typedef union {
    uint16_t raw;
    struct {
        uint8_t rc64k_calib     : 1; /* bit 0 */
        uint8_t rc13m_calib     : 1; /* bit 1 */
        uint8_t pll_calib       : 1; /* bit 2 */
        uint8_t adc_calib       : 1; /* bit 3 */
        uint8_t img_calib       : 1; /* bit 4 */
        uint8_t xosc_start      : 1; /* bit 5 */
        uint8_t pll_lock        : 1; /* bit 6 */
        uint8_t pa_ramp         : 1; /* bit 7 */
        uint8_t : 8;                 /* bits 8-15 reserved */
    };
} sx1262_reg_errors_t;

/* ── параметр калибровки ──────────────────────────────────────────────── */

typedef union {
    uint8_t raw;
    struct {
        uint8_t rc64k_calib     : 1; /* bit 0 */
        uint8_t rc13m_calib     : 1; /* bit 1 */
        uint8_t pll_calib       : 1; /* bit 2 */
        uint8_t adc_calib       : 1; /* bit 3 */
        uint8_t adc_bulk_n      : 1; /* bit 4 */
        uint8_t adc_bulk_p      : 1; /* bit 5 */
        uint8_t img_calib       : 1; /* bit 6 */
        uint8_t : 1;                 /* bit 7 reserved */
    };
} sx1262_reg_calib_t;

/* ── параметры команд ─────────────────────────────────────────────────── */

typedef enum {
    SX1262_STANDBY_RC           = 0x00,
    SX1262_STANDBY_XOSC         = 0x01,
} sx1262_standby_t;

typedef enum {
    SX1262_TCXO_1_6V            = 0x00,
    SX1262_TCXO_1_7V            = 0x01,
    SX1262_TCXO_1_8V            = 0x02,
    SX1262_TCXO_2_2V            = 0x03,
    SX1262_TCXO_2_4V            = 0x04,
    SX1262_TCXO_2_7V            = 0x05,
    SX1262_TCXO_3_0V            = 0x06,
    SX1262_TCXO_3_3V            = 0x07,
} sx1262_tcxo_voltage_t;

typedef enum {
    SX1262_RAMP_10U             = 0x00,
    SX1262_RAMP_20U             = 0x01,
    SX1262_RAMP_40U             = 0x02,
    SX1262_RAMP_80U             = 0x03,
    SX1262_RAMP_200U            = 0x04,
    SX1262_RAMP_800U            = 0x05,
    SX1262_RAMP_1700U           = 0x06,
    SX1262_RAMP_3400U           = 0x07,
} sx1262_ramp_t;

typedef enum {
    SX1262_SF7                  = 0x07,
    SX1262_SF8                  = 0x08,
    SX1262_SF9                  = 0x09,
    SX1262_SF10                 = 0x0A,
    SX1262_SF11                 = 0x0B,
    SX1262_SF12                 = 0x0C,
} sx1262_sf_t;

typedef enum {
    SX1262_BW_125               = 0x04,
    SX1262_BW_250               = 0x05,
    SX1262_BW_500               = 0x06,
} sx1262_bw_t;

typedef enum {
    SX1262_CR_4_5               = 0x01,
    SX1262_CR_4_6               = 0x02,
    SX1262_CR_4_7               = 0x03,
    SX1262_CR_4_8               = 0x04,
} sx1262_cr_t;

typedef enum {
    SX1262_LDRO_OFF             = 0x00,
    SX1262_LDRO_ON              = 0x01,
} sx1262_ldro_t;

typedef enum {
    SX1262_HEADER_EXPLICIT      = 0x00,
    SX1262_HEADER_IMPLICIT      = 0x01,
} sx1262_header_t;

typedef enum {
    SX1262_CRC_OFF              = 0x00,
    SX1262_CRC_ON               = 0x01,
} sx1262_crc_t;

typedef enum {
    SX1262_IQ_STANDARD          = 0x00,
    SX1262_IQ_INVERTED          = 0x01,
} sx1262_iq_t;

typedef enum {
    SX1262_PACKET_GFSK          = 0x00,
    SX1262_PACKET_LORA          = 0x01,
} sx1262_packet_type_t;

typedef enum {
    SX1262_PA_22DBM             = 0x00,
    SX1262_PA_20DBM             = 0x01,
    SX1262_PA_17DBM             = 0x02,
    SX1262_PA_14DBM             = 0x03,
} sx1262_pa_t;

/* ── параметры калибровки образа ──────────────────────────────────────── */

typedef struct {
    uint8_t freq_low;
    uint8_t freq_high;
} sx1262_calib_image_t;

/* ── таймаут RX ───────────────────────────────────────────────────────── */

#define SX1262_RX_CONTINUOUS 0xFFFFFF

/* ── управление ──────────────────────────────────────────────────────── */

void sx1262_init_pins(const sx1262_cfg_t * cfg);
void sx1262_reset(const sx1262_cfg_t * cfg);
void sx1262_wait_busy(const sx1262_cfg_t * cfg);

/* ── статус и регистры ────────────────────────────────────────────────── */

sx1262_reg_status_t sx1262_get_status(const sx1262_cfg_t * cfg);
uint8_t sx1262_read_reg(const sx1262_cfg_t * cfg, uint16_t addr);
void sx1262_write_reg(const sx1262_cfg_t * cfg, uint16_t addr, uint8_t val);
sx1262_reg_errors_t sx1262_get_device_errors(const sx1262_cfg_t * cfg);
void sx1262_clear_device_errors(const sx1262_cfg_t * cfg);

/* ── инициализация радио ──────────────────────────────────────────────── */

void sx1262_set_standby(const sx1262_cfg_t * cfg, sx1262_standby_t mode);
void sx1262_set_tcxo_mode(const sx1262_cfg_t * cfg, sx1262_tcxo_voltage_t voltage, uint32_t timeout_ms);
void sx1262_calibrate(const sx1262_cfg_t * cfg, sx1262_reg_calib_t mask);
void sx1262_calibrate_image(const sx1262_cfg_t * cfg, sx1262_calib_image_t img);
void sx1262_set_dio2_rf_switch_ctrl(const sx1262_cfg_t * cfg, unsigned enable);
void sx1262_set_packet_type(const sx1262_cfg_t * cfg, sx1262_packet_type_t type);
void sx1262_set_rf_frequency(const sx1262_cfg_t * cfg, uint32_t freq_hz);

/* ── настройка канала ─────────────────────────────────────────────────── */

void sx1262_set_pa_config(const sx1262_cfg_t * cfg, sx1262_pa_t pa);
void sx1262_set_tx_params(const sx1262_cfg_t * cfg, int8_t power, sx1262_ramp_t ramp_time);
void sx1262_set_mod_params(const sx1262_cfg_t * cfg, sx1262_sf_t sf, sx1262_bw_t bw, sx1262_cr_t cr, sx1262_ldro_t ldro);
void sx1262_set_packet_params(const sx1262_cfg_t * cfg, uint16_t preamble, sx1262_header_t ht, uint8_t len, sx1262_crc_t crc, sx1262_iq_t iq);
void sx1262_set_buffer_base_addr(const sx1262_cfg_t * cfg, uint8_t tx_base, uint8_t rx_base);
void sx1262_set_irq_mask(const sx1262_cfg_t * cfg, sx1262_reg_irq_t mask, unsigned dio);

/* ── передача / приём ─────────────────────────────────────────────────── */

void sx1262_write_buffer(const sx1262_cfg_t * cfg, uint8_t offset, const uint8_t * data, uint8_t len);
void sx1262_read_buffer(const sx1262_cfg_t * cfg, uint8_t offset, uint8_t * data, uint8_t len);
void sx1262_set_tx(const sx1262_cfg_t * cfg, uint32_t timeout_ms);
void sx1262_set_rx(const sx1262_cfg_t * cfg, uint32_t timeout_ms);
sx1262_reg_irq_t sx1262_get_irq_status(const sx1262_cfg_t * cfg);
void sx1262_clear_irq_status(const sx1262_cfg_t * cfg, sx1262_reg_irq_t mask);
void sx1262_get_rx_buffer_status(const sx1262_cfg_t * cfg, uint8_t * payload_len, uint8_t * rx_start_offset);
void sx1262_get_packet_status(const sx1262_cfg_t * cfg, uint8_t * rssi_raw, int8_t * snr_raw);

/* ── проверка связи ───────────────────────────────────────────────────── */

unsigned sx1262_check_connection(const sx1262_cfg_t * cfg);
