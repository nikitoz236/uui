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

/* ── аудио параметры ──────────────────────────────────── */
#define SAMPLE_RATE     16000
#define SINE_FREQ       440
#define SINE_AMP        20000
#define BUF_SAMPLES     256
#define BUF_BYTES       (BUF_SAMPLES * 2 * 2)   /* stereo, 16 бит */

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

static int16_t sine_buf[BUF_SAMPLES * 2];
static dma_desc_t dma_desc[2];

/* ── синус: 256 семплов 440 Гц @ 16 кГц, amp=20000 ───── */
/* python: [round(20000 * sin(2π * 440 * i / 16000)) for i in range(256)] */
static const int16_t sine_table[256] = {
         0,   3439,   6775,   9909,  12748,  15208,  17215,  18709,
     19646,  19998,  19754,  18922,  17526,  15609,  13226,  10450,
      7362,   4056,    628,  -2818,  -6180,  -9359, -12258, -14793,
    -16887, -18478, -19518, -19978, -19842, -19116, -17820, -15994,
    -13691, -10980,  -7943,  -4669,  -1256,   2195,   5580,   8799,
     11756,  14363,  16542,  18228,  19372,  19938,  19911,  19291,
     18097,  16363,  14142,  11500,   8516,   5277,   1882,  -1569,
     -4974,  -8230, -11242, -13918, -16180, -17961, -19206, -19879,
    -19961, -19447, -18355, -16716, -14579, -12008,  -9080,  -5881,
     -2507,    942,   4363,   7654,  10717,  13460,  15803,  17675,
     19021,  19800,  19990,  19584,  18596,  17053,  15002,  12505,
      9635,   6478,   3129,   -314,  -3748,  -7069, -10181, -12989,
    -15410, -17373, -18818, -19702, -20000, -19702, -18818, -17373,
    -15410, -12989, -10181,  -7069,  -3748,   -314,   3129,   6478,
      9635,  12505,  15002,  17053,  18596,  19584,  19990,  19800,
     19021,  17675,  15803,  13460,  10717,   7654,   4363,    942,
     -2507,  -5881,  -9080, -12008, -14579, -16716, -18355, -19447,
    -19961, -19879, -19206, -17961, -16180, -13918, -11242,  -8230,
     -4974,  -1569,   1882,   5277,   8516,  11500,  14142,  16363,
     18097,  19291,  19911,  19938,  19372,  18228,  16542,  14363,
     11756,   8799,   5580,   2195,  -1256,  -4669,  -7943, -10980,
    -13691, -15994, -17820, -19116, -19842, -19978, -19518, -18478,
    -16887, -14793, -12258,  -9359,  -6180,  -2818,    628,   4056,
      7362,  10450,  13226,  15609,  17526,  18922,  19754,  19998,
     19646,  18709,  17215,  15208,  12748,   9909,   6775,   3439,
         0,  -3439,  -6775,  -9909, -12748, -15208, -17215, -18709,
    -19646, -19998, -19754, -18922, -17526, -15609, -13226, -10450,
     -7362,  -4056,   -628,   2818,   6180,   9359,  12258,  14793,
     16887,  18478,  19518,  19978,  19842,  19116,  17820,  15994,
     13691,  10980,   7943,   4669,   1256,  -2195,  -5580,  -8799,
    -11756, -14363, -16542, -18228, -19372, -19938, -19911, -19291,
    -18097, -16363, -14142, -11500,  -8516,  -5277,  -1882,   1569,
};

static void fill_sine(void)
{
    for (int i = 0; i < BUF_SAMPLES; i++) {
        sine_buf[i * 2] = sine_table[i];
        sine_buf[i * 2 + 1] = sine_table[i];
    }
}

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

    /* заполнить синус */
    fill_sine();

    /* два дескриптора кольца: [0]→[1]→[0] */
    dma_desc[0].size = BUF_BYTES / 2;
    dma_desc[0].length = BUF_BYTES / 2;
    dma_desc[0].eof = 0;
    dma_desc[0].owner = 1;
    dma_desc[0].buf = sine_buf;
    dma_desc[0].next = &dma_desc[1];

    dma_desc[1].size = BUF_BYTES / 2;
    dma_desc[1].length = BUF_BYTES / 2;
    dma_desc[1].eof = 0;
    dma_desc[1].owner = 1;
    dma_desc[1].buf = sine_buf + BUF_SAMPLES / 2;
    dma_desc[1].next = &dma_desc[0];

    /* сброс TX FIFO I2S */
    I2S0.tx_conf.tx_reset = 1;
    I2S0.tx_conf.tx_reset = 0;
    I2S0.tx_conf.tx_fifo_reset = 1;
    I2S0.tx_conf.tx_fifo_reset = 0;

    /* старт DMA: addr[19:0] + start[21] одним write */
    GDMA.channel[0].out.link.val = ((uint32_t)&dma_desc[0] & 0xFFFFF) | (1u << 21);

    dp("[dbg] tx_conf      = 0x"); dpx(I2S0.tx_conf.val, 4); dn();
    dp("[dbg] tx_conf1     = 0x"); dpx(I2S0.tx_conf1.val, 4); dn();
    dp("[dbg] tx_clkm_conf = 0x"); dpx(I2S0.tx_clkm_conf.val, 4); dn();
    dp("[dbg] tx_tdm_ctrl  = 0x"); dpx(I2S0.tx_tdm_ctrl.val, 4); dn();

    /* старт I2S TX */
    I2S0.tx_conf.tx_update = 1;
    delay_ms(10);
    dp("[dbg] tx_update after 10ms = "); dpd(I2S0.tx_conf.tx_update, 1); dn();
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

    I2S0.int_clr.val = 0xFFFFFFFF;
    while (1) {
        delay_ms(1000);
        dp("[1s] i2s_int=0x"); dpx(I2S0.int_raw.val, 4);
        dp("  dma_int=0x"); dpx(GDMA.channel[0].out.int_raw.val, 4);
        dp("  state=0x"); dpx(GDMA.channel[0].out.state.val, 4);
        dp("  peri_sel="); dpd(GDMA.channel[0].out.peri_sel.sel, 1);
        dp("  dscr=0x"); dpx(GDMA.channel[0].out.dscr_bf0, 4);
        dn();
        I2S0.int_clr.val = 0xFFFFFFFF;
    };
}
