#include "esp32_spi.h"
#include "esp32_gpio.h"



spi_cfg_t spi = {
    .spi = &GPSPI2,
    .gpio_list = &(gpio_list_t){
        .cfg = { .mode = GPIO_MODE_MUX },
        .count = 2,
        .pin_list = {
            [SPI_PIN_MOSI] = { .pin = 35, .signal = 2 },
            [SPI_PIN_SCK] = { .pin = 36, .signal = 2 }
        }
    }
};



int main(void)
{
    init_spi(&spi);


    while (1) {};
}


