#include "periph_header.h"
#include "spi.h"
#include "stm_gpio.h"
#include "pclk.h"

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

// typedef enum {
    
// }

struct spi_cfg {
    SPI_TypeDef * spi;
    gpio_t * pin_list[SPI_PIN_NUM];
    pclk_t pclk;
    spi_clk_div_t clock_div;
    uint8_t dma_tx_ch;
    uint8_t dma_rx_ch;
};
