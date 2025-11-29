#include "esp32_spi.h"
#include "esp32_gpio.h"

#include "esp32_uart.h"

#define DP_NOTABLE
#include "dp.h"

usart_cfg_t uart2_cfg = {
    .baudrate = 115200,
    .usart = SYSTEM_UART2_CLK_EN_S + 32,
    .dev = &UART2,
    .tx_gpio = &(gpio_t){
        .cfg = { .mode = GPIO_MODE_SIG_OUT },
        .pin = { .pin = 1, .signal = U2TXD_OUT_IDX }
    }
};

spi_dev_cfg_t dev = {
    .cs_pin = &(gpio_t){
        .cfg = { .mode = GPIO_MODE_OUT },
        .pin = { .pin = 37 }
    }
};

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&uart2_cfg, s, len);
}

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
    usart_set_cfg(&uart2_cfg);
    init_spi(&spi);

    init_spi_dev(&dev);


    spi_dev_select(&dev);

    uint16_t x = 0xAB01;
    spi_set_frame_len(&spi, 16);
    spi_dma_tx_buf(&spi, &x, 1);
    spi_write_16(&spi, 0x1234);

    while (1) {};
}


