#include "esp32_gdma.h"
#include "esp32_gpio.h"
#include "esp32_i2c.h"
#include "esp32_i2s.h"
#include "dbg_usb_cdc_acm.h"
#include "delay_blocking.h"
#include "xl9555.h"
#include "es8311.h"

#define DP_NOTABLE
#include "dp.h"

#include <stdint.h>
#include <stddef.h>

void __debug_usart_tx_data(const char * s, unsigned len)
{
    dbg_usb_cdc_acm_tx(s, len);
}

/* ── I2S ──────────────────────────────────────────────── */
const i2s_cfg_t i2s = {
    .dev = &I2S0,
    .pclk = 4,   /* SYSTEM_I2S0_CLK_EN_S */
    .mclk = &(gpio_t){
        .cfg = { .mode = GPIO_MODE_SIG_OUT },
        .pin = { .signal = I2S0_MCLK_OUT_IDX, .pin = 10 },
    },
    .bclk = &(gpio_t){
        .cfg = { .mode = GPIO_MODE_SIG_OUT },
        .pin = { .signal = I2S0O_BCK_OUT_IDX, .pin = 11 },
    },
    .ws = &(gpio_t){
        .cfg = { .mode = GPIO_MODE_SIG_OUT },
        .pin = { .signal = I2S0O_WS_OUT_IDX, .pin = 18 },
    },
    .dout = &(gpio_t){
        .cfg = { .mode = GPIO_MODE_SIG_OUT },
        .pin = { .signal = I2S0O_SD_OUT_IDX, .pin = 45 },
    },
    .clk_sel = 0,    /* XTAL 40MHz */
    .mclk_div = 10,  /* MCLK = 40MHz / 10 = 4MHz */
    .bck_div = 8,    /* BCLK = 4MHz / 8 = 500kHz */
    .bits = 16,
    .dma_ch = 0,
    .dma_peri = 3,   /* GDMA peri_sel: 3 = I2S0 */
};

/* ── I2C ──────────────────────────────────────────────── */
#define I2C_SCL_PIN     2
#define I2C_SDA_PIN     3

/* ── аудио данные ─────────────────────────────────────── */
#include "reaper.h"

#define CHUNK_BYTES     4000    /* байт на один DMA дескриптор (< 4096) */
#define NUM_DESC        ((AUDIO_BUF_LEN * 2 + CHUNK_BYTES - 1) / CHUNK_BYTES)

/* ── ES8311 ───────────────────────────────────────────── */
const es8311_cfg_t es8311 = {
    .addr = 0x18,
    .bits = 16,
    .bclk_div = 4,
    .dac_osr = 0x20,
    .adc_osr = 0x10,
    .lrck_div = 0x00FF,
};

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


static gdma_desc_t dma_desc[NUM_DESC];

static void build_desc_chain(void)
{
    uint8_t * base = (uint8_t *)audio_buf;
    unsigned total = AUDIO_BUF_LEN * 2;
    for (unsigned i = 0; i < NUM_DESC; i++) {
        unsigned off = i * CHUNK_BYTES;
        unsigned sz = total - off;
        if (sz > CHUNK_BYTES) {
            sz = CHUNK_BYTES;
        }
        dma_desc[i].size = sz;
        dma_desc[i].length = sz;
        dma_desc[i].eof = (i == NUM_DESC - 1) ? 1 : 0;
        dma_desc[i].owner = 1;
        dma_desc[i].buf = base + off;
        dma_desc[i].next = (i == NUM_DESC - 1) ? NULL : &dma_desc[i + 1];
    }
}

/* ── main ─────────────────────────────────────────────── */
int main(void)
{
    dpn("tlora audio i2s");

    gdma_init();
    init_i2c(&i2c);
    dpn("[init] i2c ok");

    xl9555_gpio_set(&amp_en, 0);
    init_xl9555_gpio(&amp_en);

    init_i2s(&i2s);
    dpn("[init] i2s ok");

    dp("ES8311 chip_id = 0x"); dpx(es8311_read_chipid(&es8311), 1); dn();

    init_es8311(&es8311);
    dpn("[init] es8311 ok");

    xl9555_gpio_set(&amp_en, 1);
    dpn("[init] amp on");

    delay_ms(1000);

    /* громкость до запуска DMA — ни один семпл не пропадёт */
    es8311_set_volume(&es8311, 0xBF);
    build_desc_chain();
    i2s_tx_start(&i2s, &dma_desc[0]);
    dpn("[init] dma+i2s started");

    while (1) {}
}
