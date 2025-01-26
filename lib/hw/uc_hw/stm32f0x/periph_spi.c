/*
    STM32F0 SPI implementation
*/

#include "periph_spi.h"
#include "../stm_dma.h"

void init_spi(const spi_cfg_t * cfg)
{
    pclk_ctrl(&cfg->pclk, 1);

    for (unsigned i = 0; i < SPI_PIN_NUM; i++) {
        init_gpio(cfg->pin_list[i]);
    }

    cfg->spi->CR1 = 0;
    cfg->spi->CR2 = 0;

    cfg->spi->CR1 |= SPI_CR1_MSTR;      // Master configuration
    cfg->spi->CR1 |= SPI_CR1_SSM;       // Software slave management
    cfg->spi->CR1 |= SPI_CR1_SSI;       // Internal slave select


    // cfg->spi->CR2 |= SPI_CR2_SSOE;

    cfg->spi->CR1 |= SPI_CR1_BR_0 * cfg->clock_div;

    // cfg->spi->CR2 |= SPI_CR2_FRXTH;
    if (cfg->dma_tx_ch) {
        cfg->spi->CR2 |= SPI_CR2_TXDMAEN;
        dma_channel(cfg->dma_tx_ch)->CCR = 0;
        dma_set_periph_tx(cfg->dma_tx_ch, (void *)&cfg->spi->DR);
    }

    if (cfg->dma_rx_ch) {
        cfg->spi->CR2 |= SPI_CR2_RXDMAEN;
        dma_channel(cfg->dma_rx_ch)->CCR = 0;
        dma_set_periph_rx(cfg->dma_rx_ch, (void *)&cfg->spi->DR);
    }

/*
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

    len--;

    // If software attempts to write one of the “Not used” values, they are forced to the value “0111”
    // тоесть мы не можем стереть данные биты в 0, по этому ставим их в 1, потом сбрасываем ненужные
    // нет требования выключать приемопередатчик перед изменением
    cfg->spi->CR2 |= SPI_CR2_DS_0 * 0x0F;
    cfg->spi->CR2 &= (~SPI_CR2_DS) | (SPI_CR2_DS_0 * len);
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

void spi_dma_tx_buf(const spi_cfg_t * cfg, const void * txdata, unsigned len)
{
    // с инкрементом данных в буфере
    while (dma_get_cnt(cfg->dma_tx_ch) != 0) {};
    dma_stop(cfg->dma_tx_ch);
    dma_enable_mem_inc(cfg->dma_tx_ch);
    dma_start(cfg->dma_tx_ch, txdata, len);
}

void spi_dma_tx_repeat(const spi_cfg_t * cfg, const void * txdata, unsigned len)
{
    // без инкремента одно и тоже значение
    while (dma_get_cnt(cfg->dma_tx_ch) != 0) {};
    dma_stop(cfg->dma_tx_ch);
    dma_disable_mem_inc(cfg->dma_tx_ch);
    dma_start(cfg->dma_tx_ch, txdata, len);
}
