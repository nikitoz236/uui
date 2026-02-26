#pragma once

#include <stdint.h>
#include "gpio.h"

struct spi_cfg;
typedef const struct spi_cfg spi_cfg_t;

typedef struct {
    gpio_t * cs_pin;
    spi_cfg_t * spi;
} spi_dev_cfg_t;

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

static inline void init_spi_dev(const spi_dev_cfg_t * cfg)
{
    init_gpio(cfg->cs_pin);
    gpio_set_state(cfg->cs_pin, 1);
}

static inline void spi_dev_select(const spi_dev_cfg_t * cfg)
{
    gpio_set_state(cfg->cs_pin, 0);
}

static inline void spi_dev_unselect(const spi_dev_cfg_t * cfg)
{
    while (spi_is_busy(cfg->spi)) {}
    gpio_set_state(cfg->cs_pin, 1);
}
void spi_set_frame_len(const spi_cfg_t * cfg, unsigned len);

uint8_t spi_exchange_8(const spi_cfg_t * cfg, uint8_t c);

void spi_write_8(const spi_cfg_t * cfg, uint8_t c);
void spi_write_16(const spi_cfg_t * cfg, uint16_t c);

void spi_dma_tx_buf(const spi_cfg_t * cfg, const void * txdata, unsigned len);
void spi_dma_tx_repeat(const spi_cfg_t * cfg, const void * txdata, unsigned len);

/*
    вообще как будто бы у нас должны быть асинхронные вызовы

    типа зарядить дма, дальше у нас дма дернет прерывание по окончанию отправки.
*/
