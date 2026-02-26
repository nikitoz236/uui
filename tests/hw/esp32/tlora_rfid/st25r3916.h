#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "spi.h"

/*
 * ST25R3916 NFC reader — минимальный bare-metal драйвер.
 *
 * SPI протокол:
 *   Байт 0 (mode/addr):
 *     [7:6]=00  Register Write,  [5:0]=addr
 *     [7:6]=01  Register Read,   [5:0]=addr
 *     [7:6]=11  Direct Command,  [5:0]=cmd code
 *     0x80      FIFO Load
 *     0x9F      FIFO Read
 *     0xFB      Space-B access (перед R/W space-B регистров)
 */

/* ── Config ──────────────────────────────────────────────────── */

typedef struct {
    spi_dev_cfg_t spi_dev;
    const gpio_t * irq;        /* IRQ pin (input), может быть NULL для polling */
} st25r3916_cfg_t;

/* ── Register addresses (Space A) ────────────────────────────── */

#define ST_REG_IO_CONF1             0x00
#define ST_REG_IO_CONF2             0x01
#define ST_REG_OP_CONTROL           0x02
#define ST_REG_MODE                 0x03
#define ST_REG_BIT_RATE             0x04
#define ST_REG_ISO14443A_NFC        0x05
#define ST_REG_ISO14443B_1          0x06
#define ST_REG_ISO14443B_2          0x07
#define ST_REG_PASSIVE_TARGET       0x08
#define ST_REG_STREAM_MODE          0x09
#define ST_REG_AUX                  0x0A
#define ST_REG_RX_CONF1             0x0B
#define ST_REG_RX_CONF2             0x0C
#define ST_REG_RX_CONF3             0x0D
#define ST_REG_RX_CONF4             0x0E
#define ST_REG_MASK_RX_TIMER        0x0F
#define ST_REG_NO_RESPONSE_TIMER1   0x10
#define ST_REG_NO_RESPONSE_TIMER2   0x11
#define ST_REG_TIMER_EMV_CONTROL    0x12
#define ST_REG_GPT1                 0x13
#define ST_REG_GPT2                 0x14
#define ST_REG_PPON2                0x15
#define ST_REG_IRQ_MASK_MAIN        0x16
#define ST_REG_IRQ_MASK_TIMER_NFC   0x17
#define ST_REG_IRQ_MASK_ERROR_WUP   0x18
#define ST_REG_IRQ_MASK_TARGET      0x19
#define ST_REG_IRQ_MAIN             0x1A
#define ST_REG_IRQ_TIMER_NFC        0x1B
#define ST_REG_IRQ_ERROR_WUP        0x1C
#define ST_REG_IRQ_TARGET           0x1D
#define ST_REG_FIFO_STATUS1         0x1E
#define ST_REG_FIFO_STATUS2         0x1F
#define ST_REG_COLLISION_STATUS     0x20
#define ST_REG_PASSIVE_TARGET_STATUS 0x21
#define ST_REG_NUM_TX_BYTES1        0x22
#define ST_REG_NUM_TX_BYTES2        0x23
#define ST_REG_AD_RESULT            0x25
#define ST_REG_ANT_TUNE_A           0x26
#define ST_REG_ANT_TUNE_B           0x27
#define ST_REG_TX_DRIVER            0x28
#define ST_REG_PT_MOD               0x29
#define ST_REG_FIELD_THRESHOLD_ACTV 0x2A
#define ST_REG_FIELD_THRESHOLD_DEACTV 0x2B
#define ST_REG_REGULATOR_CONTROL    0x2C
#define ST_REG_RSSI_RESULT          0x2D
#define ST_REG_GAIN_RED_STATE       0x2E
#define ST_REG_CAP_SENSOR_CONTROL   0x2F
#define ST_REG_AUX_DISPLAY          0x31
#define ST_REG_EMD_SUP_CONF         0x33
#define ST_REG_AUX_MOD             0x34
#define ST_REG_IC_IDENTITY          0x3F

/* Space-B registers (prefix with 0xFB command before SPI R/W) */
#define ST_SPACE_B                  0x40    /* OR with reg addr for Space-B */
#define ST_REG_CORR_CONF1          (ST_SPACE_B | 0x0C)
#define ST_REG_CORR_CONF2          (ST_SPACE_B | 0x0D)

/* ── Register bit definitions ────────────────────────────────── */

/* OP_CONTROL */
#define ST_OP_EN            (1u << 7)   /* Oscillator enable */
#define ST_OP_RX_EN         (1u << 6)   /* Receiver enable */
#define ST_OP_TX_EN         (1u << 3)   /* Transmitter enable (field on) */
#define ST_OP_WU            (1u << 2)   /* Wake-up mode */

/* MODE */
#define ST_MODE_TARG        (1u << 7)   /* 1=target, 0=initiator */
#define ST_MODE_OM_MASK     (0xFu << 3)
#define ST_MODE_OM_NFC      (0x0u << 3)
#define ST_MODE_OM_14443A   (0x1u << 3)
#define ST_MODE_OM_14443B   (0x2u << 3)
#define ST_MODE_OM_FELICA   (0x3u << 3)
#define ST_MODE_OM_TOPAZ    (0x4u << 3)
#define ST_MODE_NFC_AR_OFF  (0x0u)
#define ST_MODE_NFC_AR_AUTO (0x1u)

/* BIT_RATE */
#define ST_BR_TX_106        (0x0u << 4)
#define ST_BR_TX_212        (0x1u << 4)
#define ST_BR_RX_106        (0x0u << 0)
#define ST_BR_RX_212        (0x1u << 0)

/* IO_CONF2 */
#define ST_IO2_SUP3V        (1u << 7)
#define ST_IO2_AAT_EN       (1u << 5)   /* Auto Antenna Tuning enable */
#define ST_IO2_MISO_PD2     (1u << 4)
#define ST_IO2_MISO_PD1     (1u << 3)
#define ST_IO2_IO_DRV_LVL   (1u << 2)

/* ISO14443A_NFC */
#define ST_14443A_NO_TX_PAR (1u << 7)
#define ST_14443A_NO_RX_PAR (1u << 6)
#define ST_14443A_ANTCL     (1u << 0)

/* TIMER_EMV_CONTROL */
#define ST_NRT_STEP_64FC    (0u << 0)
#define ST_NRT_STEP_4096FC  (1u << 0)

/* AUX */
#define ST_AUX_NO_CRC_RX    (1u << 7)   /* Disable CRC check on RX */
#define ST_AUX_DIS_CORR     (1u << 2)   /* 0=correlator, 1=coherent */

/* AUX_DISPLAY */
#define ST_AUX_OSC_OK       (1u << 4)

/* IRQ_MAIN (reg 0x1A) */
#define ST_IRQ_OSC          (1u << 7)   /* Oscillator stable */
#define ST_IRQ_FWL          (1u << 6)   /* FIFO water level */
#define ST_IRQ_RXS          (1u << 5)   /* RX start */
#define ST_IRQ_RXE          (1u << 4)   /* RX end */
#define ST_IRQ_TXE          (1u << 3)   /* TX end */
#define ST_IRQ_COL          (1u << 2)   /* Bit collision */
#define ST_IRQ_RX_REST      (1u << 1)   /* Auto reception restart */

/* IRQ_TIMER_NFC (reg 0x1B) */
#define ST_IRQ_DCT          (1u << 7)   /* Direct command terminated */
#define ST_IRQ_NRE          (1u << 6)   /* No-response timer expire */
#define ST_IRQ_GPE          (1u << 5)   /* GP timer expire */
#define ST_IRQ_EON          (1u << 4)   /* External field on */
#define ST_IRQ_EOF          (1u << 3)   /* External field off */
#define ST_IRQ_CAC          (1u << 2)   /* Collision avoidance */
#define ST_IRQ_CAT          (1u << 1)   /* Min guard time expired */
#define ST_IRQ_NFCT         (1u << 0)   /* Initiator bit rate recognised */

/* IRQ_ERROR_WUP (reg 0x1C) */
#define ST_IRQ_CRC          (1u << 7)   /* CRC error */
#define ST_IRQ_PAR          (1u << 6)   /* Parity error */
#define ST_IRQ_ERR2         (1u << 5)   /* Soft framing error */
#define ST_IRQ_ERR1         (1u << 4)   /* Hard framing error */
#define ST_IRQ_WT           (1u << 3)   /* Wake-up timer */
#define ST_IRQ_WAM          (1u << 2)   /* Wake-up amplitude */
#define ST_IRQ_WPH          (1u << 1)   /* Wake-up phase */
#define ST_IRQ_WCAP         (1u << 0)   /* Wake-up capacitance */

/* FIFO_STATUS2 */
#define ST_FIFO2_B_MASK     (0x03u << 6)
#define ST_FIFO2_B_SHIFT    6

/* IC_IDENTITY */
#define ST_IC_TYPE_MASK      0xF8u
#define ST_IC_TYPE_ST25R3916  0x28u     /* 00101 << 3 */
#define ST_IC_TYPE_ST25R3916B 0xC0u     /* 11000 << 3 */
#define ST_IC_REV_MASK       0x07u

/* ── Direct Commands ─────────────────────────────────────────── */

#define ST_CMD_SET_DEFAULT          0xC1
#define ST_CMD_STOP                 0xC2
#define ST_CMD_TX_WITH_CRC          0xC4
#define ST_CMD_TX_WITHOUT_CRC       0xC5
#define ST_CMD_TX_REQA              0xC6
#define ST_CMD_TX_WUPA              0xC7
#define ST_CMD_MASK_RX_DATA         0xD0
#define ST_CMD_UNMASK_RX_DATA       0xD1
#define ST_CMD_MEASURE_AMPLITUDE    0xD3
#define ST_CMD_RESET_RXGAIN         0xD5
#define ST_CMD_ADJUST_REGULATORS    0xD6
#define ST_CMD_CLEAR_RSSI           0xDA
#define ST_CMD_CLEAR_FIFO           0xDB
#define ST_CMD_MEASURE_VDD          0xDF
#define ST_CMD_SPACE_B_ACCESS       0xFB

/* ── ISO 14443A defines ──────────────────────────────────────── */

#define NFCA_CMD_REQA               0x26
#define NFCA_CMD_WUPA               0x52
#define NFCA_SEL_CL1                0x93
#define NFCA_SEL_CL2                0x95
#define NFCA_SEL_CL3                0x97
#define NFCA_NVB_ANTICOL            0x20
#define NFCA_NVB_SELECT             0x70

#define NFCA_CASCADE_TAG            0x88
#define NFCA_MAX_UID_LEN            10

/* ── NFC-A card info ─────────────────────────────────────────── */

typedef struct {
    uint8_t uid[NFCA_MAX_UID_LEN];
    uint8_t uid_len;            /* 4, 7 или 10 */
    uint8_t atqa[2];
    uint8_t sak;
} nfca_card_t;

/* ── API ─────────────────────────────────────────────────────── */

/* Low-level SPI */
void     st_write_reg(const st25r3916_cfg_t * cfg, uint8_t reg, uint8_t val);
uint8_t  st_read_reg(const st25r3916_cfg_t * cfg, uint8_t reg);
void     st_set_reg_bits(const st25r3916_cfg_t * cfg, uint8_t reg, uint8_t mask);
void     st_clr_reg_bits(const st25r3916_cfg_t * cfg, uint8_t reg, uint8_t mask);
void     st_modify_reg(const st25r3916_cfg_t * cfg, uint8_t reg, uint8_t clr, uint8_t set);
void     st_exec_cmd(const st25r3916_cfg_t * cfg, uint8_t cmd);
void     st_write_fifo(const st25r3916_cfg_t * cfg, const uint8_t * data, unsigned len);
unsigned st_read_fifo(const st25r3916_cfg_t * cfg, uint8_t * buf, unsigned max_len);

/* Interrupt register polling (reads and clears) */
uint8_t  st_get_irq_main(const st25r3916_cfg_t * cfg);
uint8_t  st_get_irq_timer(const st25r3916_cfg_t * cfg);
uint8_t  st_get_irq_error(const st25r3916_cfg_t * cfg);

/* Init & ID */
bool     st_check_id(const st25r3916_cfg_t * cfg, uint8_t * rev);
bool     st_init(const st25r3916_cfg_t * cfg);

/* NFC-A */
void     st_field_on(const st25r3916_cfg_t * cfg);
void     st_field_off(const st25r3916_cfg_t * cfg);
bool     st_nfca_poll(const st25r3916_cfg_t * cfg, nfca_card_t * card);
