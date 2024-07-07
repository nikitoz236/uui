#pragma once

#include <stdint.h>
#include "gpio.h"

struct spi_cfg;
typedef struct spi_cfg spi_cfg_t;

typedef struct {
    const spi_cfg_t * spi;
    gpio_pin_t cs_pin;
    gpio_cfg_t * cs_pin_cfg;
} spi_slave_cfg_t;

enum spi_gpio_list {
    SPI_PIN_SCK,
    SPI_PIN_MOSI,
    SPI_PIN_MISO,
    SPI_PIN_CS,

    SPI_PIN_NUM,
    SPI_PIN_NUM_2_PIN_TX = SPI_PIN_MOSI + 1,
    SPI_PIN_NUM_3_PIN_RX_TX = SPI_PIN_MISO + 1,
    SPI_PIN_NUM_4_PIN_RX_TX_CS = SPI_PIN_CS + 1,
};

void init_spi(const spi_cfg_t * cfg);
unsigned spi_is_busy(const spi_cfg_t * cfg);
void spi_set_frame_len(const spi_cfg_t * cfg, unsigned len);


void spi_write_8(const spi_cfg_t * cfg, uint8_t c);
void spi_write_16(const spi_cfg_t * cfg, uint16_t c);



void spi_tx16_blocking(const spi_cfg_t * cfg, uint16_t data);

void spi_tx16_dma_buf(const spi_slave_cfg_t * cfg, uint16_t * txdata, unsigned len);
void spi_tx16_dma_repeat(const spi_slave_cfg_t * cfg, uint16_t val, unsigned repeat);
