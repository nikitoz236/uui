#include "spi.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

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
    hw_pclk_t pclk;
    spi_clk_div_t clock_div;
    gpio_pin_t pin_list[SPI_PIN_NUM];
    gpio_cfg_t pin_cfg[SPI_PIN_NUM];
};
