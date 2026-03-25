#include "esp32_gpio.h"
#include "esp32_i2c.h"
#include "esp32_pclk.h"
#include "dbg_usb_cdc_acm.h"
#include "delay_blocking.h"
#include "xl9555.h"

#define DP_NOTABLE
#include "dp.h"

#include "soc/i2s_struct.h"
#include "soc/gdma_struct.h"

#include <stdint.h>
#include <stddef.h>

void __debug_usart_tx_data(const char * s, unsigned len)
{
    dbg_usb_cdc_acm_tx(s, len);
}

/* ── пины ─────────────────────────────────────────────── */
#define I2C_SCL_PIN     2
#define I2C_SDA_PIN     3
#define I2S_MCLK_PIN    10
#define I2S_BCLK_PIN    11
#define I2S_WS_PIN      18
#define I2S_DOUT_PIN    45

/* ── аудио данные ─────────────────────────────────────── */
#include "reaper.h"

#define CHUNK_BYTES     4000    /* байт на один DMA дескриптор (< 4096) */
#define NUM_DESC        ((AUDIO_BUF_LEN * 2 + CHUNK_BYTES - 1) / CHUNK_BYTES)

/* ── ES8311 I2C адрес ─────────────────────────────────── */
#define ES8311_ADDR     0x18

/* ── I2C ──────────────────────────────────────────────── */
const i2c_cfg_t i2c = {
    .dev = &I2C0,
    .i2c_pclk = 7,   /* SYSTEM_I2C_EXT0_CLK_EN_S */
    .freq = 100000,
    .pin_list = &(gpio_list_t){
        .count = 2,
        .cfg = { .mode = GPIO_MODE_SIG_IO, .pu = 1, .od = 1 },
        .pin_list = {
            { .signal = I2CEXT0_SCL_IN_IDX, .pin = I2C_SCL_PIN },
            { .signal = I2CEXT0_SDA_IN_IDX, .pin = I2C_SDA_PIN }
        }
    }
};

/* ── XL9555: усилитель NS4150B на GPIO1 ───────────────── */
const xl9555_gpio_t amp_en = {
    .dir = XL9555_DIR_OUT,
    .pin = 1
};

/* ── ES8311: запись/чтение регистра ───────────────────── */
static void es_wr(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = { reg, val };
    i2c_transaction(ES8311_ADDR, buf, 2, 0, 0);
    while (i2c_status() == I2C_STATUS_BUSY) {};
}

static uint8_t es_rd(uint8_t reg)
{
    uint8_t val = 0;
    i2c_transaction(ES8311_ADDR, &reg, 1, &val, 1);
    while (i2c_status() == I2C_STATUS_BUSY) {};
    return val;
}

/* ── ES8311: инициализация ────────────────────────────── */
/*
 * Последовательность из референсного драйвера LilyGo:
 * es8311_open → set_bits_per_sample(16) → config_sample(16kHz)
 * → es8311_start(DAC) → set_mute(false) → set_vol(0dB)
 */
static void es8311_init(void)
{
    uint8_t regv;
    uint8_t chip_id = 0;

    i2c_transaction(ES8311_ADDR, &(uint8_t){0xFD}, 1, &chip_id, 1);
    while (i2c_status() == I2C_STATUS_BUSY) {};
    dp("ES8311 chip_id = 0x"); dpx(chip_id, 1); dn();

    /* workaround: первый write может игнорироваться */
    es_wr(0x44, 0x08);
    es_wr(0x44, 0x08);

    /* clock manager defaults */
    es_wr(0x01, 0x30);
    es_wr(0x02, 0x00);
    es_wr(0x03, 0x10);
    es_wr(0x16, 0x24);
    es_wr(0x04, 0x10);
    es_wr(0x05, 0x00);
    es_wr(0x0B, 0x00);
    es_wr(0x0C, 0x00);
    es_wr(0x10, 0x1F);
    es_wr(0x11, 0x7F);

    /* CSM reset pulse */
    es_wr(0x00, 0x80);

    /* slave mode (bit6=0) */
    regv = es_rd(0x00);
    regv &= 0xBF;
    es_wr(0x00, regv);

    /* MCLK from external pin, not inverted */
    es_wr(0x01, 0x3F);

    /* BCLK not inverted */
    regv = es_rd(0x06);
    regv &= ~0x20;
    es_wr(0x06, regv);

    /* reference bias, ADC HPF */
    es_wr(0x13, 0x10);
    es_wr(0x1B, 0x0A);
    es_wr(0x1C, 0x6A);

    /* internal DAC reference */
    es_wr(0x44, 0x58);

    /* set_bits_per_sample(16): DAC SDP = Philips I2S, 16-bit */
    regv = es_rd(0x09);
    regv &= 0xFC;
    regv |= 0x0C;
    es_wr(0x09, regv);

    regv = es_rd(0x0A);
    regv &= 0xFC;
    regv |= 0x0C;
    es_wr(0x0A, regv);

    /* config_sample(16kHz): MCLK=4.096MHz, Fs=16kHz
     * coeff: pre_div=1, pre_mult=1, adc_div=1, dac_div=1
     *        lrck=0x00FF, bclk_div=4, adc_osr=0x10, dac_osr=0x20 */
    regv = es_rd(0x02);
    regv &= 0x07;
    es_wr(0x02, regv);

    es_wr(0x05, 0x00);

    regv = es_rd(0x03);
    regv &= 0x80;
    regv |= 0x10;
    es_wr(0x03, regv);

    regv = es_rd(0x04);
    regv &= 0x80;
    regv |= 0x20;
    es_wr(0x04, regv);

    regv = es_rd(0x07);
    regv &= 0xC0;
    es_wr(0x07, regv);

    es_wr(0x08, 0xFF);

    regv = es_rd(0x06);
    regv &= 0xE0;
    regv |= (4 - 1);
    es_wr(0x06, regv);

    /* es8311_start(DAC): CSM reset + slave */
    es_wr(0x00, 0x80 & 0xBF);

    /* MCLK from pin */
    es_wr(0x01, 0x3F);

    /* DAC SDP active, ADC tri-state */
    regv = es_rd(0x09);
    regv &= 0xBF;
    es_wr(0x09, regv);

    regv = es_rd(0x0A);
    regv &= 0xBF;
    regv |= 0x40;
    es_wr(0x0A, regv);

    es_wr(0x17, 0xBF);   /* ADC volume 0 dB */
    es_wr(0x0E, 0x02);   /* HP bias on */
    es_wr(0x12, 0x00);   /* DAC enable */
    es_wr(0x14, 0x1A);   /* analog PGA */
    es_wr(0x0D, 0x01);   /* VDDA reference on */
    es_wr(0x15, 0x40);   /* ADC ramp rate */
    es_wr(0x37, 0x08);   /* DAC ramp rate */
    es_wr(0x45, 0x00);   /* GP normal */

    /* set_mute(false) */
    regv = es_rd(0x31);
    regv &= 0x9F;
    es_wr(0x31, regv);

    /* set_vol(0 dB) */
    es_wr(0x32, 0xBF);

    dpn("ES8311 init OK");
}

/* ── I2S: инициализация ───────────────────────────────── */
/*
 * STD (Philips), master, 16-bit stereo, 16 kHz
 * MCLK = PLL160M / 39.0625 = 4.096 MHz  (256 × 16 kHz)
 * BCLK = MCLK / 8 = 512 kHz             (32 × 16 kHz)
 * WS   = BCLK / 32 = 16 kHz
 *
 * PLL160M fractional div: N=39, b/a=1/16 (b<=a/2):
 *   z=1, y=0, x=15, yn1=0
 */
static void i2s_init(void)
{
    /* тактирование */
    pclk_ctrl(4, 1);   /* SYSTEM_I2S0_CLK_EN_S = 4 */
    pclk_reset(4);

    /* GPIO: MCLK, BCLK, WS, DOUT */
    init_gpio(&(gpio_t){
        .cfg = { .mode = GPIO_MODE_SIG_OUT },
        .pin = { .signal = I2S0_MCLK_OUT_IDX, .pin = I2S_MCLK_PIN }
    });
    init_gpio(&(gpio_t){
        .cfg = { .mode = GPIO_MODE_SIG_OUT },
        .pin = { .signal = I2S0O_BCK_OUT_IDX, .pin = I2S_BCLK_PIN }
    });
    init_gpio(&(gpio_t){
        .cfg = { .mode = GPIO_MODE_SIG_OUT },
        .pin = { .signal = I2S0O_WS_OUT_IDX, .pin = I2S_WS_PIN }
    });
    init_gpio(&(gpio_t){
        .cfg = { .mode = GPIO_MODE_SIG_OUT },
        .pin = { .signal = I2S0O_SD_OUT_IDX, .pin = I2S_DOUT_PIN }
    });

    /* clock: XTAL(40MHz)/10 = 4MHz (тест: без PLL)
     * tx_clkm_conf: div_num=10[7:0], clk_active=1[26], clk_sel=0/XTAL[28:27], clk_en=1[29] */
    I2S0.tx_clkm_conf.val = 10 | (1u << 26) | (0u << 27) | (1u << 29);
    I2S0.tx_clkm_div_conf.val = 0;
    /* rx_clkm_conf.mclk_sel=0 (reset default): MCLK_OUT = TX clock */

    /* TX формат */
    I2S0.tx_conf1.tx_bck_div_num = 7;           /* BCLK = MCLK/8: 8-1=7 */
    I2S0.tx_conf1.tx_bits_mod = 15;             /* 16-bit data: 16-1=15 */
    I2S0.tx_conf1.tx_tdm_chan_bits = 15;        /* channel slot = 16 бит */
    I2S0.tx_conf1.tx_half_sample_bits = 15;    /* STD: slot_bit_width-1 */
    I2S0.tx_conf1.tx_tdm_ws_width = 15;        /* WS = 16 BCLK: 16-1=15 */
    I2S0.tx_conf1.tx_msb_shift = 1;            /* Philips: сдвиг MSB */
    I2S0.tx_conf1.tx_bck_no_dly = 0;

    /* TX режим: master, TDM (=STD), pcm_bypass, WS=0→L, MSB first
     * tx_conf: pcm_bypass=1[12], tdm_en=1[19] */
    I2S0.tx_conf.val = (1u << 12) | (1u << 19);

    /* TDM: 2 канала (L+R)
     * tx_tdm_ctrl: chan0_en=1[0], chan1_en=1[1], tot_chan_num=1[19:16] */
    I2S0.tx_tdm_ctrl.val = (1u << 0) | (1u << 1) | (1u << 16);

}

/* ── DMA дескриптор ───────────────────────────────────── */
typedef struct dma_desc {
    volatile uint32_t size : 12, length : 12, offset : 5, sosf : 1, eof : 1, owner : 1;
    void * buf;
    struct dma_desc * next;
} dma_desc_t;

static dma_desc_t dma_desc[NUM_DESC];

/* ── GDMA: инициализация и запуск ─────────────────────── */
/*
 * GDMA channel 0 TX → I2S0 (peri_sel=3)
 * Один дескриптор замкнут на себя: DMA крутит синус вечно.
 */
static void gdma_start(void)
{
    pclk_ctrl(32 + SYSTEM_DMA_CLK_EN_S, 1);
    pclk_reset(32 + SYSTEM_DMA_CLK_EN_S);   /* default RST=1, снимаем reset */
    GDMA.misc_conf.clk_en = 1;   /* force enable reg clock */

    /* сброс TX-канала 0 */
    GDMA.channel[0].out.conf0.out_rst = 1;
    GDMA.channel[0].out.conf0.out_rst = 0;

    /* подключить к I2S0 */
    GDMA.channel[0].out.peri_sel.sel = 3;

    /* цепочка дескрипторов по CHUNK_BYTES, последний с eof=1 */
    uint8_t * base = (uint8_t *)audio_buf;
    unsigned total = AUDIO_BUF_LEN * 2;   /* байт */
    for (int i = 0; i < NUM_DESC; i++) {
        unsigned off = i * CHUNK_BYTES;
        unsigned sz = total - off;
        if (sz > CHUNK_BYTES) sz = CHUNK_BYTES;
        dma_desc[i].size = sz;
        dma_desc[i].length = sz;
        dma_desc[i].eof = (i == NUM_DESC - 1) ? 1 : 0;
        dma_desc[i].owner = 1;
        dma_desc[i].buf = base + off;
        dma_desc[i].next = (i == NUM_DESC - 1) ? NULL : &dma_desc[i + 1];
    }

    /* сброс TX FIFO I2S */
    I2S0.tx_conf.tx_reset = 1;
    I2S0.tx_conf.tx_reset = 0;
    I2S0.tx_conf.tx_fifo_reset = 1;
    I2S0.tx_conf.tx_fifo_reset = 0;

    /* старт DMA: addr[19:0] + start[21] одним write */
    GDMA.channel[0].out.link.val = ((uint32_t)&dma_desc[0] & 0xFFFFF) | (1u << 21);

    /* старт I2S TX */
    I2S0.tx_conf.tx_update = 1;
    while (I2S0.tx_conf.tx_update) {}
    I2S0.tx_conf.tx_start = 1;
}

/* ── main ─────────────────────────────────────────────── */
int main(void)
{
    dpn("tlora audio i2s");

    init_i2c(&i2c);
    dpn("[init] i2c ok");

    i2s_init();
    dpn("[init] i2s ok (MCLK running)");

    es8311_init();

    init_xl9555_gpio(&amp_en);
    xl9555_gpio_set(&amp_en, 1);
    dpn("[init] amp on");

    gdma_start();
    dpn("[init] dma+i2s started");

    while (1) {}
}
