#include "esp32_i2s.h"

void init_i2s(const i2s_cfg_t * cfg)
{
    pclk_ctrl(cfg->pclk, 1);
    pclk_reset(cfg->pclk);

    init_gpio(cfg->mclk);
    init_gpio(cfg->bclk);
    init_gpio(cfg->ws);
    init_gpio(cfg->dout);

    /* тактирование: MCLK = clk_src / mclk_div */
    {
        typeof(cfg->dev->tx_clkm_conf) v = { .val = 0 };
        v.tx_clkm_div_num = cfg->mclk_div;
        v.tx_clk_active = 1;
        v.tx_clk_sel = cfg->clk_sel;
        v.clk_en = 1;
        cfg->dev->tx_clkm_conf.val = v.val;
    }
    cfg->dev->tx_clkm_div_conf.val = 0;

    /* формат: STD/TDM, Philips I2S, 2 канала */
    {
        uint8_t b = cfg->bits - 1;
        typeof(cfg->dev->tx_conf1) v = { .val = 0 };
        v.tx_bck_div_num = cfg->bck_div - 1;
        v.tx_bits_mod = b;
        v.tx_tdm_chan_bits = b;
        v.tx_half_sample_bits = b;
        v.tx_tdm_ws_width = b;
        v.tx_msb_shift = 1;
        v.tx_bck_no_dly = 0;
        cfg->dev->tx_conf1.val = v.val;
    }

    /* pcm_bypass + tdm_en */
    {
        typeof(cfg->dev->tx_conf) v = { .val = 0 };
        v.tx_pcm_bypass = 1;
        v.tx_tdm_en = 1;
        cfg->dev->tx_conf.val = v.val;
    }

    /* TDM каналы: моно = только chan0, стерео = chan0 + chan1 */
    {
        typeof(cfg->dev->tx_tdm_ctrl) v = { .val = 0 };
        v.tx_tdm_chan0_en = 1;
        if (cfg->channels >= 2) {
            v.tx_tdm_chan1_en = 1;
        }
        v.tx_tdm_tot_chan_num = 1;
        cfg->dev->tx_tdm_ctrl.val = v.val;
    }

}

void i2s_tx_start(const i2s_cfg_t * cfg, const gdma_desc_t * desc)
{
    cfg->dev->tx_conf.tx_reset = 1;
    cfg->dev->tx_conf.tx_reset = 0;
    cfg->dev->tx_conf.tx_fifo_reset = 1;
    cfg->dev->tx_conf.tx_fifo_reset = 0;

    gdma_tx_start(cfg->dma_ch, cfg->dma_peri, desc);

    cfg->dev->tx_conf.tx_update = 1;
    while (cfg->dev->tx_conf.tx_update) {};
    cfg->dev->tx_conf.tx_start = 1;
}
