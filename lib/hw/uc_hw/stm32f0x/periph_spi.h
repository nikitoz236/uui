/*
    STM32F0 SPI implementation
*/

#include "periph_header.h"
#include "spi.h"
#include "gpio.h"
#include "pclk.h"

// тут реально все одинаковое с F1, кроме установки размера фрейма, нехватае полнодуплексных транзакций

typedef enum {
    SPI_DIV_2 = 0,
    SPI_DIV_4 = 1,
    SPI_DIV_8 = 2,
    SPI_DIV_16 = 3,
    SPI_DIV_32 = 4,
    SPI_DIV_64 = 5,
    SPI_DIV_128 = 6,
    SPI_DIV_256 = 7,
} spi_clk_div_t;

struct spi_cfg {
    SPI_TypeDef * spi;
    gpio_t * pin_list[SPI_PIN_NUM];
    pclk_t pclk;
    spi_clk_div_t clock_div;
    uint8_t dma_tx_ch;
    uint8_t dma_rx_ch;
};
