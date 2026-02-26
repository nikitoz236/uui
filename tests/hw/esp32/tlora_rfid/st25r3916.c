#include "st25r3916.h"
#include "delay_blocking.h"

#define DP_NOTABLE
#include "dp.h"

/* ── SPI protocol constants ──────────────────────────────────── */

#define SPI_WRITE_MODE  (0u << 6)
#define SPI_READ_MODE   (1u << 6)
#define SPI_CMD_MODE    (3u << 6)
#define SPI_FIFO_LOAD   0x80u
#define SPI_FIFO_READ   0x9Fu
#define SPI_SPACE_B     0xFBu

/* ── Low-level SPI ───────────────────────────────────────────── */

void st_write_reg(const st25r3916_cfg_t * cfg, uint8_t reg, uint8_t val)
{
    spi_dev_select(&cfg->spi_dev);
    if (reg & ST_SPACE_B)
        spi_write_8(cfg->spi_dev.spi, SPI_SPACE_B);
    spi_write_8(cfg->spi_dev.spi, (reg & ~ST_SPACE_B) | SPI_WRITE_MODE);
    spi_write_8(cfg->spi_dev.spi, val);
    spi_dev_unselect(&cfg->spi_dev);
}

uint8_t st_read_reg(const st25r3916_cfg_t * cfg, uint8_t reg)
{
    spi_dev_select(&cfg->spi_dev);
    if (reg & ST_SPACE_B)
        spi_write_8(cfg->spi_dev.spi, SPI_SPACE_B);
    spi_write_8(cfg->spi_dev.spi, (reg & ~ST_SPACE_B) | SPI_READ_MODE);
    uint8_t val = spi_exchange_8(cfg->spi_dev.spi, 0x00);
    spi_dev_unselect(&cfg->spi_dev);
    return val;
}

void st_set_reg_bits(const st25r3916_cfg_t * cfg, uint8_t reg, uint8_t mask)
{
    uint8_t val = st_read_reg(cfg, reg);
    st_write_reg(cfg, reg, val | mask);
}

void st_clr_reg_bits(const st25r3916_cfg_t * cfg, uint8_t reg, uint8_t mask)
{
    uint8_t val = st_read_reg(cfg, reg);
    st_write_reg(cfg, reg, val & ~mask);
}

void st_modify_reg(const st25r3916_cfg_t * cfg, uint8_t reg, uint8_t clr, uint8_t set)
{
    uint8_t val = st_read_reg(cfg, reg);
    val &= ~clr;
    val |= set;
    st_write_reg(cfg, reg, val);
}

void st_exec_cmd(const st25r3916_cfg_t * cfg, uint8_t cmd)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, cmd);  /* cmd уже содержит [7:6]=11 */
    spi_dev_unselect(&cfg->spi_dev);
}

void st_write_fifo(const st25r3916_cfg_t * cfg, const uint8_t * data, unsigned len)
{
    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SPI_FIFO_LOAD);
    for (unsigned i = 0; i < len; i++)
        spi_write_8(cfg->spi_dev.spi, data[i]);
    spi_dev_unselect(&cfg->spi_dev);
}

unsigned st_read_fifo(const st25r3916_cfg_t * cfg, uint8_t * buf, unsigned max_len)
{
    /* Получаем кол-во байт в FIFO */
    uint8_t s1 = st_read_reg(cfg, ST_REG_FIFO_STATUS1);
    uint8_t s2 = st_read_reg(cfg, ST_REG_FIFO_STATUS2);
    unsigned fifo_len = s1 | ((unsigned)(s2 & ST_FIFO2_B_MASK) << (8 - ST_FIFO2_B_SHIFT));

    if (fifo_len > max_len)
        fifo_len = max_len;

    if (fifo_len == 0)
        return 0;

    spi_dev_select(&cfg->spi_dev);
    spi_write_8(cfg->spi_dev.spi, SPI_FIFO_READ);
    for (unsigned i = 0; i < fifo_len; i++)
        buf[i] = spi_exchange_8(cfg->spi_dev.spi, 0x00);
    spi_dev_unselect(&cfg->spi_dev);

    return fifo_len;
}

/* ── Interrupt register polling ──────────────────────────────── */

uint8_t st_get_irq_main(const st25r3916_cfg_t * cfg)
{
    return st_read_reg(cfg, ST_REG_IRQ_MAIN);
}

uint8_t st_get_irq_timer(const st25r3916_cfg_t * cfg)
{
    return st_read_reg(cfg, ST_REG_IRQ_TIMER_NFC);
}

uint8_t st_get_irq_error(const st25r3916_cfg_t * cfg)
{
    return st_read_reg(cfg, ST_REG_IRQ_ERROR_WUP);
}

/* ── Ожидание IRQ с таймаутом (polling) ──────────────────────── */

static uint8_t wait_irq_main(const st25r3916_cfg_t * cfg, uint8_t mask, unsigned timeout_ms)
{
    for (unsigned t = 0; t < timeout_ms; t++) {
        uint8_t irq = st_get_irq_main(cfg);
        if (irq & mask)
            return irq;
        delay_ms(1);
    }
    return 0;
}

static uint8_t wait_irq_timer(const st25r3916_cfg_t * cfg, uint8_t mask, unsigned timeout_ms)
{
    for (unsigned t = 0; t < timeout_ms; t++) {
        uint8_t irq = st_get_irq_timer(cfg);
        if (irq & mask)
            return irq;
        delay_ms(1);
    }
    return 0;
}

/* ── Init & ID ───────────────────────────────────────────────── */

bool st_check_id(const st25r3916_cfg_t * cfg, uint8_t * rev)
{
    uint8_t id = st_read_reg(cfg, ST_REG_IC_IDENTITY);
    dp("IC_IDENTITY: "); dpx(id, 1); dn();

    if (rev)
        *rev = id & ST_IC_REV_MASK;

    uint8_t type = id & ST_IC_TYPE_MASK;
    return (type == ST_IC_TYPE_ST25R3916) || (type == ST_IC_TYPE_ST25R3916B);
}

bool st_init(const st25r3916_cfg_t * cfg)
{
    init_spi_dev(&cfg->spi_dev);

    if (cfg->irq)
        init_gpio(cfg->irq);

    /* Дадим чипу время после power-on */
    delay_ms(10);

    /* Пробный read (пробуждаем SPI) */
    st_read_reg(cfg, ST_REG_IC_IDENTITY);

    /* Set default state */
    st_exec_cmd(cfg, ST_CMD_SET_DEFAULT);
    delay_ms(10);

    /* Увеличить MISO drive level для быстрого SPI */
    st_write_reg(cfg, ST_REG_IO_CONF2, ST_IO2_IO_DRV_LVL | ST_IO2_SUP3V);

    /* Check chip ID */
    uint8_t rev;
    if (!st_check_id(cfg, &rev)) {
        dpn("  st25r3916 ID MISMATCH");
        return false;
    }
    dp("  rev "); dpd(rev, 1); dn();

    /* Enable MISO pull-downs + AAT */
    st_set_reg_bits(cfg, ST_REG_IO_CONF2,
                    ST_IO2_MISO_PD1 | ST_IO2_MISO_PD2 | ST_IO2_AAT_EN);

    /* Enable oscillator */
    st_set_reg_bits(cfg, ST_REG_OP_CONTROL, ST_OP_EN);

    /* Ожидаем стабилизации осциллятора (~700мкс, max 10мс) */
    for (unsigned t = 0; t < 20; t++) {
        uint8_t aux = st_read_reg(cfg, ST_REG_AUX_DISPLAY);
        if (aux & ST_AUX_OSC_OK) {
            dpn("oscillator ok");
            break;
        }
        delay_ms(1);
    }

    /* Очистить все IRQ */
    st_get_irq_main(cfg);
    st_get_irq_timer(cfg);
    st_get_irq_error(cfg);
    st_read_reg(cfg, ST_REG_IRQ_TARGET);

    /* Unmask all interrupts (для polling — маскируем всё, читаем статус напрямую) */
    st_write_reg(cfg, ST_REG_IRQ_MASK_MAIN, 0xFF);
    st_write_reg(cfg, ST_REG_IRQ_MASK_TIMER_NFC, 0xFF);
    st_write_reg(cfg, ST_REG_IRQ_MASK_ERROR_WUP, 0xFF);
    st_write_reg(cfg, ST_REG_IRQ_MASK_TARGET, 0xFF);

    /* Adjust regulators */
    st_exec_cmd(cfg, ST_CMD_ADJUST_REGULATORS);
    delay_ms(10);

    /* Field thresholds: activation=105mV, deactivation=75mV
     * (trg nibble in [7:4], rfe nibble in [3:0]) */
    st_write_reg(cfg, ST_REG_FIELD_THRESHOLD_ACTV, 0x11);   /* trg=105mV, rfe=105mV */
    st_write_reg(cfg, ST_REG_FIELD_THRESHOLD_DEACTV, 0x00);  /* trg=75mV,  rfe=75mV  */

    /* TX driver: d_res=0 (active transmit) */
    st_modify_reg(cfg, ST_REG_TX_DRIVER, 0xC0, 0x00);

    dpn("st25r3916 init ok");
    return true;
}

/* ── Field control ───────────────────────────────────────────── */

void st_field_on(const st25r3916_cfg_t * cfg)
{
    /* Включить TX (field) и RX */
    st_set_reg_bits(cfg, ST_REG_OP_CONTROL, ST_OP_TX_EN | ST_OP_RX_EN);
    delay_ms(10);  /* Guard time for field stabilization */

    /* Adjust regulators после включения поля */
    st_exec_cmd(cfg, ST_CMD_ADJUST_REGULATORS);
    delay_ms(10);

    uint8_t op = st_read_reg(cfg, ST_REG_OP_CONTROL);
    dp("  OP_CTRL=0x"); dpx(op, 1); dn();
}

void st_field_off(const st25r3916_cfg_t * cfg)
{
    st_clr_reg_bits(cfg, ST_REG_OP_CONTROL, ST_OP_TX_EN | ST_OP_RX_EN);
}

/* ── NFC-A helpers ───────────────────────────────────────────── */

static void nfca_setup_mode(const st25r3916_cfg_t * cfg)
{
    /* ISO 14443A, initiator, 106 kbps, OOK modulation, auto-rx off */
    st_write_reg(cfg, ST_REG_MODE, ST_MODE_OM_14443A | ST_MODE_NFC_AR_OFF);
    st_write_reg(cfg, ST_REG_BIT_RATE, ST_BR_TX_106 | ST_BR_RX_106);

    /* Enable anticollision support */
    st_write_reg(cfg, ST_REG_ISO14443A_NFC, ST_14443A_ANTCL);

    /* Use correlator receiver (not coherent) */
    st_clr_reg_bits(cfg, ST_REG_AUX, ST_AUX_DIS_CORR);

    /* RX config from RFAL analog config table (NFC-A 106kbps RX) */
    st_write_reg(cfg, ST_REG_RX_CONF1, 0x08);
    st_write_reg(cfg, ST_REG_RX_CONF2, 0x2D);
    st_write_reg(cfg, ST_REG_RX_CONF3, 0x00);
    st_write_reg(cfg, ST_REG_RX_CONF4, 0x00);

    /* Correlator config (Space-B registers) */
    st_write_reg(cfg, ST_REG_CORR_CONF1, 0x51);
    st_write_reg(cfg, ST_REG_CORR_CONF2, 0x00);

    /* TX driver: AM off (OOK), d_res=0 */
    st_write_reg(cfg, ST_REG_TX_DRIVER, 0x00);

    /* No-response timer: step=64/fc ≈ 4.72мкс, value=0x0800 ≈ 9.7мс */
    st_write_reg(cfg, ST_REG_TIMER_EMV_CONTROL, ST_NRT_STEP_64FC);
    st_write_reg(cfg, ST_REG_NO_RESPONSE_TIMER1, 0x08);
    st_write_reg(cfg, ST_REG_NO_RESPONSE_TIMER2, 0x00);

    /* Mask RX timer: default */
    st_write_reg(cfg, ST_REG_MASK_RX_TIMER, 0x00);
}

/*
 * Отправить REQA (7-бит short frame) через direct command.
 * Возвращает true если получен ATQA (2 байта).
 */
static bool nfca_send_reqa(const st25r3916_cfg_t * cfg, uint8_t atqa[2])
{
    /* Очистить FIFO и IRQ */
    st_exec_cmd(cfg, ST_CMD_CLEAR_FIFO);
    st_get_irq_main(cfg);
    st_get_irq_timer(cfg);
    st_get_irq_error(cfg);

    /* Прямая команда TX_REQA — чип сам отправит 7-бит REQA frame */
    st_exec_cmd(cfg, ST_CMD_TX_REQA);

    /* Ждём RXE (ответ) или таймаут. После TX чип автоматически
     * переходит в режим приёма и запускает NRT */
    uint8_t irq = 0, tirq = 0, eirq = 0;
    for (unsigned t = 0; t < 20; t++) {
        irq |= st_get_irq_main(cfg);
        tirq |= st_get_irq_timer(cfg);
        eirq |= st_get_irq_error(cfg);

        if (irq & ST_IRQ_RXE)
            break;
        if (tirq & ST_IRQ_NRE)
            break;

        delay_ms(1);
    }

    (void)eirq;

    if (!(irq & ST_IRQ_RXE))
        return false;

    /* Читаем ATQA из FIFO (2 байта) */
    unsigned n = st_read_fifo(cfg, atqa, 2);
    return (n >= 2);
}

/*
 * Anticollision loop для одного cascade level.
 * sel_cmd: 0x93 (CL1), 0x95 (CL2), 0x97 (CL3)
 * uid_out: 4 байта UID + BCC
 * Возвращает true при успешном получении.
 */
static bool nfca_anticollision(const st25r3916_cfg_t * cfg,
                               uint8_t sel_cmd, uint8_t uid_bcc[5])
{
    st_exec_cmd(cfg, ST_CMD_CLEAR_FIFO);
    st_get_irq_main(cfg);
    st_get_irq_timer(cfg);
    st_get_irq_error(cfg);

    /* Disable CRC check on RX (SDD response has no CRC) */
    st_set_reg_bits(cfg, ST_REG_AUX, ST_AUX_NO_CRC_RX);

    /* Write anticollision frame: SEL + NVB(0x20) = 2 bytes = 16 bits */
    uint8_t frame[2] = { sel_cmd, NFCA_NVB_ANTICOL };
    st_write_fifo(cfg, frame, 2);

    /* Set number of TX bits = 16 */
    st_write_reg(cfg, ST_REG_NUM_TX_BYTES2, (2u << 3));  /* 2 bytes, 0 bits */
    st_write_reg(cfg, ST_REG_NUM_TX_BYTES1, 0);

    /* Transmit without CRC (parity is still active — ISO 14443A always uses parity) */
    st_exec_cmd(cfg, ST_CMD_TX_WITHOUT_CRC);

    uint8_t irq = wait_irq_main(cfg, ST_IRQ_RXE | ST_IRQ_COL, 10);

    if (irq & ST_IRQ_COL) {
        dpn("  collision detected");
        st_clr_reg_bits(cfg, ST_REG_AUX, ST_AUX_NO_CRC_RX);
        return false;
    }

    if (!(irq & ST_IRQ_RXE)) {
        st_clr_reg_bits(cfg, ST_REG_AUX, ST_AUX_NO_CRC_RX);
        return false;
    }

    unsigned n = st_read_fifo(cfg, uid_bcc, 5);

    /* Re-enable CRC on RX */
    st_clr_reg_bits(cfg, ST_REG_AUX, ST_AUX_NO_CRC_RX);

    if (n < 5)
        return false;

    /* Verify BCC */
    uint8_t bcc = uid_bcc[0] ^ uid_bcc[1] ^ uid_bcc[2] ^ uid_bcc[3];
    if (bcc != uid_bcc[4]) {
        dpn("  BCC mismatch");
        return false;
    }

    return true;
}

/*
 * SELECT command для одного cascade level.
 * sel_cmd: 0x93/0x95/0x97
 * uid_bcc: 5 байт (4 UID + BCC)
 * sak_out: 1 байт SAK
 */
static bool nfca_select(const st25r3916_cfg_t * cfg,
                        uint8_t sel_cmd, const uint8_t uid_bcc[5], uint8_t * sak_out)
{
    st_exec_cmd(cfg, ST_CMD_CLEAR_FIFO);
    st_get_irq_main(cfg);
    st_get_irq_timer(cfg);
    st_get_irq_error(cfg);

    /* SELECT frame: SEL + NVB(0x70) + 4 bytes UID + BCC = 7 bytes */
    uint8_t frame[7] = {
        sel_cmd, NFCA_NVB_SELECT,
        uid_bcc[0], uid_bcc[1], uid_bcc[2], uid_bcc[3], uid_bcc[4]
    };
    st_write_fifo(cfg, frame, 7);

    /* TX bits = 7 bytes = 56 bits */
    st_write_reg(cfg, ST_REG_NUM_TX_BYTES2, (7u << 3));
    st_write_reg(cfg, ST_REG_NUM_TX_BYTES1, 0);

    /* Transmit with CRC (ST25R3916 добавит CRC_A автоматически) */
    st_exec_cmd(cfg, ST_CMD_TX_WITH_CRC);

    uint8_t irq = wait_irq_main(cfg, ST_IRQ_RXE, 10);
    if (!(irq & ST_IRQ_RXE))
        return false;

    /* SAK = 1 byte (+ 2 CRC, но CRC снимается автоматически если no_crc_rx не установлен) */
    uint8_t resp[3];
    unsigned n = st_read_fifo(cfg, resp, 3);
    if (n < 1)
        return false;

    *sak_out = resp[0];
    return true;
}

/* ── NFC-A Poll ──────────────────────────────────────────────── */

bool st_nfca_poll(const st25r3916_cfg_t * cfg, nfca_card_t * card)
{
    nfca_setup_mode(cfg);

    /* REQA */
    if (!nfca_send_reqa(cfg, card->atqa))
        return false;

    card->uid_len = 0;

    /* Cascade level 1 */
    uint8_t uid_bcc[5];
    if (!nfca_anticollision(cfg, NFCA_SEL_CL1, uid_bcc))
        return false;

    uint8_t sak;
    if (!nfca_select(cfg, NFCA_SEL_CL1, uid_bcc, &sak))
        return false;

    if (uid_bcc[0] == NFCA_CASCADE_TAG) {
        /* UID не полный, продолжаем CL2 */
        card->uid[0] = uid_bcc[1];
        card->uid[1] = uid_bcc[2];
        card->uid[2] = uid_bcc[3];
        card->uid_len = 3;

        /* Cascade level 2 */
        if (!nfca_anticollision(cfg, NFCA_SEL_CL2, uid_bcc))
            return false;
        if (!nfca_select(cfg, NFCA_SEL_CL2, uid_bcc, &sak))
            return false;

        if (uid_bcc[0] == NFCA_CASCADE_TAG) {
            /* Triple-size UID, CL3 */
            card->uid[3] = uid_bcc[1];
            card->uid[4] = uid_bcc[2];
            card->uid[5] = uid_bcc[3];
            card->uid_len = 6;

            if (!nfca_anticollision(cfg, NFCA_SEL_CL3, uid_bcc))
                return false;
            if (!nfca_select(cfg, NFCA_SEL_CL3, uid_bcc, &sak))
                return false;

            card->uid[6] = uid_bcc[0];
            card->uid[7] = uid_bcc[1];
            card->uid[8] = uid_bcc[2];
            card->uid[9] = uid_bcc[3];
            card->uid_len = 10;
        } else {
            /* Double-size UID (7 bytes) */
            card->uid[3] = uid_bcc[0];
            card->uid[4] = uid_bcc[1];
            card->uid[5] = uid_bcc[2];
            card->uid[6] = uid_bcc[3];
            card->uid_len = 7;
        }
    } else {
        /* Single-size UID (4 bytes) */
        card->uid[0] = uid_bcc[0];
        card->uid[1] = uid_bcc[1];
        card->uid[2] = uid_bcc[2];
        card->uid[3] = uid_bcc[3];
        card->uid_len = 4;
    }

    card->sak = sak;
    return true;
}
