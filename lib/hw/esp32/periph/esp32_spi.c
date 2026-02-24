#include "esp32_spi.h"
#include "round_up.h"

// #define DP_OFF
#include "dp.h"

static unsigned spi_len = 1;

#define SPI_1MHZ

void init_spi(const spi_cfg_t * cfg)
{
    for (unsigned i = 0; i < SPI_PIN_NUM; i++) {
        if (cfg->pin_list[i]) {
            init_gpio(cfg->pin_list[i]);
        }
    }

#if defined SPI_1MHZ
    cfg->spi->clock.clk_equ_sysclk = 0;
    cfg->spi->clock.clkdiv_pre = 1 - 1;
    cfg->spi->clock.clkcnt_n = 40 - 1;
    cfg->spi->clock.clkcnt_h = 20 - 1;
    cfg->spi->clock.clkcnt_l = 40 - 1;
#else
    cfg->spi->clock.clk_equ_sysclk = 1;
#endif

    cfg->spi->clk_gate.clk_en = 1;
    cfg->spi->clk_gate.mst_clk_active = 1;

    cfg->spi->user.doutdin = 1;

    cfg->spi->user.usr_command = 0;
    cfg->spi->user.usr_addr = 0;
    cfg->spi->user.usr_mosi = 1;
    cfg->spi->user.usr_miso = 1;
    cfg->spi->user.cs_setup = 0;
    cfg->spi->user.cs_hold = 0;
    cfg->spi->user.usr_dummy = 0;
    cfg->spi->cmd.conf_bitlen = 0;

    // жестко задаем 8 для начала. потом переопределим
    cfg->spi->ms_dlen.ms_data_bitlen = 8 - 1;

    // cfg->spi->misc.clk_data_dtr_en = 1;

    cfg->spi->user1.usr_addr_bitlen = 0;
    cfg->spi->user1.usr_dummy_cyclelen = 0;


    cfg->spi->dma_int_set.trans_done_int_set = 1;

    cfg->spi->cmd.update = 1;
    while (cfg->spi->cmd.update);
}

void spi_tx(const spi_cfg_t * cfg, uint8_t * data, unsigned bit_len)
{
    while (spi_is_busy(cfg)) {};

    cfg->spi->dma_int_clr.trans_done = 1;

    cfg->spi->ms_dlen.ms_data_bitlen = bit_len - 1;
    unsigned data_buf_idx = 0;
    unsigned bytes = bit_len + 7;
    bytes >>= 3;

    while(bytes) {
        uint32_t tmp = 0;
        uint8_t * ptr = (uint8_t *)&tmp;
        for (unsigned i = 0; i < sizeof(uint32_t); i++) {
            if (bytes) {
                *ptr++ = *data++;
                bytes--;
            }
        }
        // dp("SPI tmp: "); dpx(tmp, 4); dn();
        cfg->spi->data_buf[data_buf_idx++] = tmp;
    }

    // dpx(cfg->spi->data_buf[0], 4);
    cfg->spi->cmd.update = 1;
    while (cfg->spi->cmd.update);
    cfg->spi->cmd.usr = 1;
}


void spi_run(const spi_cfg_t * cfg)
{
    cfg->spi->dma_int_clr.trans_done = 1;
    cfg->spi->cmd.update = 1;
    while (cfg->spi->cmd.update);
    cfg->spi->cmd.usr = 1;
}


unsigned spi_is_busy(const spi_cfg_t * cfg)
{
    if (cfg->spi->dma_int_raw.trans_done == 0) {
        return 1;
    }
    return 0;
}


void spi_set_frame_len(const spi_cfg_t * cfg, unsigned len)
{
    while (spi_is_busy(cfg)) {};
    spi_len = round_up_deg2(len, 8) / 8;
    cfg->spi->ms_dlen.ms_data_bitlen = len - 1;
}

void spi_tx_oct(const spi_cfg_t * cfg, void * data)
{
    while (spi_is_busy(cfg)) {};

    uint32_t tmp = 0;
    for (unsigned i = 0; i < spi_len; i++) {
        tmp <<= 8;
        tmp |= *((uint8_t *)data + i);
    }
    cfg->spi->data_buf[0] = tmp;

    spi_run(cfg);
}

uint8_t spi_exchange_8(const spi_cfg_t * cfg, uint8_t c)
{
    spi_tx_oct(cfg, &c);
    while (spi_is_busy(cfg)) {};
    // dp("   spi exchange 8, data: "); dpxd(cfg->spi->data_buf, 4, 16); dn();

    return (uint8_t)(cfg->spi->data_buf[0] & 0xFF);
}

void spi_write_8(const spi_cfg_t * cfg, uint8_t c)
{
    spi_tx_oct(cfg, &c);
}

void spi_write_16(const spi_cfg_t * cfg, uint16_t c)
{
    spi_tx_oct(cfg, &c);
}

void spi_dma_tx_buf(const spi_cfg_t * cfg, const void * txdata, unsigned len)
{
    while (len) {
        spi_tx_oct(cfg, txdata);
        txdata += spi_len;
        len--;
    }
}

void spi_dma_tx_repeat(const spi_cfg_t * cfg, const void * txdata, unsigned len)
{
    while (len) {
        spi_tx_oct(cfg, txdata);
        len--;
    }
}
