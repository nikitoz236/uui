#include "stm32f10x.h"
#include "stm32f10x_spi.h"

void init_spi(const spi_cfg_t * cfg)
{
    hw_rcc_pclk_ctrl(&cfg->pclk, 1);

    for (unsigned i = 0; i < SPI_PIN_NUM; i++) {
        if (cfg->pin_list[i].port != GPIO_EMPTY) {
            gpio_set_cfg(&cfg->pin_list[i], &cfg->pin_cfg[i]);
        }
    }

    cfg->spi->CR1 = 0;
    cfg->spi->CR2 = 0;

    cfg->spi->CR1 |= SPI_CR1_MSTR;      // Master configuration
    cfg->spi->CR1 |= SPI_CR1_SSM;       // Software slave management
    cfg->spi->CR1 |= SPI_CR1_SSI;       // Internal slave select


    // cfg->spi->CR2 |= SPI_CR2_SSOE;

    cfg->spi->CR1 |= SPI_CR1_BR_0 * cfg->clock_div;

    // cfg->spi->CR2 |= SPI_CR2_FRXTH;
/*
    if (cfg->dma_tx_ch) {
        cfg->spi->CR2 |= SPI_CR2_TXDMAEN;
        dma_ch_regs(cfg->dma_tx_ch)->CPAR = (uint32_t)&cfg->spi->DR;
    }

    if (cfg->dma_rx_ch) {
        cfg->spi->CR2 |= SPI_CR2_RXDMAEN;
        dma_ch_regs(cfg->dma_tx_ch)->CPAR = (uint32_t)&cfg->spi->DR;
    }

    if (cfg->size) {
        spi_set_frame_len(hw, cfg->size);
    }
*/

    cfg->spi->CR1 |= SPI_CR1_SPE;

}

unsigned spi_is_busy(const spi_cfg_t * cfg)
{
    if (cfg->spi->SR & SPI_SR_BSY) {
        return 1;
    }
    return 0;
}

void spi_set_frame_len(const spi_cfg_t * cfg, unsigned len)
{
    if (len > 16) {
        return;
    }
    if (len < 4) {
        return;
    }

    while (spi_is_busy(cfg)) {};

    cfg->spi->CR1 &= ~(SPI_CR1_SPE + SPI_CR1_DFF);
    if (len == 16) {
        cfg->spi->CR1 |= SPI_CR1_DFF;
    }
    cfg->spi->CR1 |= SPI_CR1_SPE;
}

void spi_write_8(const spi_cfg_t * cfg, uint8_t c)
{
    while(!(cfg->spi->SR & SPI_SR_TXE)) {};
    *(uint8_t*)&cfg->spi->DR = c;
}

void spi_write_16(const spi_cfg_t * cfg, uint16_t c)
{
    while(!(cfg->spi->SR & SPI_SR_TXE)) {};
    cfg->spi->DR = c;
}


void spi_tx16_dma_buf(const spi_slave_cfg_t * cfg, uint16_t * txdata, unsigned len);
void spi_tx16_dma_repeat(const spi_slave_cfg_t * cfg, uint16_t val, unsigned repeat);
