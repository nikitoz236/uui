#include "es8311.h"
#include "i2c.h"

static void wr(uint8_t addr, uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = { reg, val };
    i2c_transaction(addr, buf, 2, 0, 0);
    while (i2c_status() == I2C_STATUS_BUSY) {};
}

static uint8_t rd(uint8_t addr, uint8_t reg)
{
    uint8_t val = 0;
    i2c_transaction(addr, &reg, 1, &val, 1);
    while (i2c_status() == I2C_STATUS_BUSY) {};
    return val;
}

static uint8_t bits_to_sdp(uint8_t bits)
{
    switch (bits) {
    case 16: return 0x0C;
    case 24: return 0x00;
    case 32: return 0x10;
    default: return 0x0C;
    }
}

void init_es8311(const es8311_cfg_t * cfg)
{
    uint8_t a = cfg->addr;
    uint8_t regv;

    /* workaround: первый write может игнорироваться */
    wr(a, 0x44, 0x08);
    wr(a, 0x44, 0x08);

    /* clock manager defaults */
    wr(a, 0x01, 0x30);
    wr(a, 0x02, 0x00);
    wr(a, 0x03, 0x10);
    wr(a, 0x16, 0x24);
    wr(a, 0x04, 0x10);
    wr(a, 0x05, 0x00);
    wr(a, 0x0B, 0x00);
    wr(a, 0x0C, 0x00);
    wr(a, 0x10, 0x1F);
    wr(a, 0x11, 0x7F);

    /* CSM reset pulse */
    wr(a, 0x00, 0x80);

    /* slave mode (bit6=0) */
    regv = rd(a, 0x00);
    regv &= 0xBF;
    wr(a, 0x00, regv);

    /* MCLK from external pin, not inverted */
    wr(a, 0x01, 0x3F);

    /* BCLK not inverted */
    regv = rd(a, 0x06);
    regv &= ~0x20;
    wr(a, 0x06, regv);

    /* reference bias, ADC HPF */
    wr(a, 0x13, 0x10);
    wr(a, 0x1B, 0x0A);
    wr(a, 0x1C, 0x6A);

    /* internal DAC reference */
    wr(a, 0x44, 0x58);

    /* SDP: Philips I2S, configured bit depth */
    uint8_t sdp = bits_to_sdp(cfg->bits);

    regv = rd(a, 0x09);
    regv &= 0xE0;
    regv |= sdp;
    wr(a, 0x09, regv);

    regv = rd(a, 0x0A);
    regv &= 0xE0;
    regv |= sdp;
    wr(a, 0x0A, regv);

    /* sample rate: dividers from config */
    regv = rd(a, 0x02);
    regv &= 0x07;
    wr(a, 0x02, regv);

    wr(a, 0x05, 0x00);

    regv = rd(a, 0x03);
    regv &= 0x80;
    regv |= cfg->adc_osr;
    wr(a, 0x03, regv);

    regv = rd(a, 0x04);
    regv &= 0x80;
    regv |= cfg->dac_osr;
    wr(a, 0x04, regv);

    regv = rd(a, 0x07);
    regv &= 0xC0;
    regv |= (cfg->lrck_div >> 8) & 0x3F;
    wr(a, 0x07, regv);

    wr(a, 0x08, cfg->lrck_div & 0xFF);

    regv = rd(a, 0x06);
    regv &= 0xE0;
    regv |= (cfg->bclk_div - 1);
    wr(a, 0x06, regv);

    /* start DAC: CSM power on, slave mode */
    wr(a, 0x00, 0x80);
    wr(a, 0x01, 0x3F);

    /* DAC SDP active, ADC tri-state */
    regv = rd(a, 0x09);
    regv &= 0xBF;
    wr(a, 0x09, regv);

    regv = rd(a, 0x0A);
    regv &= 0xBF;
    regv |= 0x40;
    wr(a, 0x0A, regv);

    wr(a, 0x17, 0xBF);   /* ADC volume 0 dB */
    wr(a, 0x0E, 0x02);   /* HP bias on */
    wr(a, 0x12, 0x00);   /* DAC enable */
    wr(a, 0x14, 0x1A);   /* analog PGA */
    wr(a, 0x0D, 0x01);   /* VDDA reference on */
    wr(a, 0x15, 0x40);   /* ADC ramp rate */
    wr(a, 0x37, 0x08);   /* DAC ramp rate */
    wr(a, 0x45, 0x00);   /* GP normal */

    /* mute + volume 0 после инициализации.
       вызывающий код сам решает когда поднять громкость */
    wr(a, 0x32, 0x00);
}

void es8311_set_mute(const es8311_cfg_t * cfg, unsigned mute)
{
    uint8_t regv = rd(cfg->addr, 0x31);
    regv &= 0x9F;
    if (mute) {
        regv |= 0x60;
    }
    wr(cfg->addr, 0x31, regv);
}

void es8311_set_volume(const es8311_cfg_t * cfg, uint8_t vol)
{
    wr(cfg->addr, 0x32, vol);
}

uint8_t es8311_read_chipid(const es8311_cfg_t * cfg)
{
    return rd(cfg->addr, 0xFD);
}
