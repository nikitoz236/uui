#include "config.h"

const rcc_cfg_t rcc_cfg = {
    .hse_val = 8000000,
    .pll_src = PLL_SRC_PREDIV,
    .pll_prediv = 1,
    .pll_mul = 9,
    .sysclk_src = SYSCLK_SRC_PLL,
    .hclk_div = HCLK_DIV1,
    .apb_div = {
        APB_DIV2,
        APB_DIV1
    }
};

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&debug_usart, s, len);
}

#define DEBUG_USART_TX_BUF_SIZE 64

struct {    // TODO тоже в библиотеку
    rb_t rb;
    uint8_t data[DEBUG_USART_TX_BUF_SIZE];
} debug_usart_dma_tx_ctx;

void debug_usart_dma_hander(void)
{
    usart_dma_tx_end_handler(&debug_usart);
}

const usart_cfg_t debug_usart = {
    .usart = USART1,
    .default_baud = 115200,
    .rx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 10
        },
        .cfg = {
            .mode = GPIO_MODE_INPUT,
            .pull = GPIO_PULL_NONE,
        }
    },
    .tx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 9
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
        }
    },
    .tx_dma = {
        .dma_ch = 4,
        .size = DEBUG_USART_TX_BUF_SIZE,
        .rb = &debug_usart_dma_tx_ctx.rb
    },
    .tx_dma_handler = debug_usart_dma_hander,
    .pclk = PCLK_USART1,
    .irqn = USART1_IRQn
};

const usart_cfg_t kline_usart = {
    .usart = USART3,
    .default_baud = 9600,
    .rx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_B,
            .pin = 11
        },
        .cfg = {
            .mode = GPIO_MODE_INPUT,
            .pull = GPIO_PULL_NONE,
        }
    },
    .tx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_B,
            .pin = 10
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
        }
    },
    .tx_dma = {
        .dma_ch = 2,
        .size = 0,
    },
    .rx_dma = {
        .dma_ch = 3,
        .size = 0,
    },
    .pclk = PCLK_USART3,
//     .irqn = USART3_IRQn
};


const lcd_cfg_t lcd_cfg = {
    .ctrl_lines = &(gpio_list_t){
        .cfg = {
            .mode = GPIO_MODE_OUTPUT,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP
        },
        .count = 2,
        .pin_list = (gpio_pin_t[]){
            [LCD_DC] = {GPIO_PORT_A, 11},
            [LCD_RST] = {GPIO_PORT_A, 12}
        }
    },
    .spi_dev = {
        .cs_pin = &(gpio_t){
            .gpio = {GPIO_PORT_A, 4},
            .cfg = {
                .mode = GPIO_MODE_OUTPUT,
                .speed = GPIO_SPEED_HIGH,
                .type = GPIO_TYPE_PP
            },
        },
        .spi = &(spi_cfg_t){
            .spi = SPI1,
            .dma_tx_ch = 3,
        .pclk = PCLK_SPI1,
            .pin_list = {
                [SPI_PIN_SCK] = &(gpio_t){ .gpio = {GPIO_PORT_A, 5}, .cfg = { .mode = GPIO_MODE_AF, .type = GPIO_TYPE_PP, .speed = GPIO_SPEED_HIGH }},
                [SPI_PIN_MISO] = &(gpio_t){ .gpio = {GPIO_PORT_A, 6}, .cfg = { .mode = GPIO_MODE_AF, .pull = GPIO_PULL_NONE }},
                [SPI_PIN_MOSI] = &(gpio_t){ .gpio = {GPIO_PORT_A, 7}, .cfg = { .mode = GPIO_MODE_AF, .type = GPIO_TYPE_PP, .speed = GPIO_SPEED_HIGH }},
            },
            // .clock_div = SPI_DIV_64
            .clock_div = SPI_DIV_2
        },
    },
    .bl = &(pwm_cfg_t){
        .freq = 40000,
        .max_val = 10,
        .ch = 2,
        .tim = TIM3,
        .pclk = PCLK_TIM3,
        .gpio = &(gpio_t){
            .gpio = {
                .port = GPIO_PORT_B,
                .pin = 5
            },
            .cfg = {
                .mode = GPIO_MODE_AF,
                .speed = GPIO_SPEED_HIGH,
                .type = GPIO_TYPE_PP
            }
        }
    },
};
