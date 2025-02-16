#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "periph_gpio.h"

#include "systick.h"
#include "stm_pwm.h"
#include "stm_usart.h"

#include "mstimer.h"
#include "delay_blocking.h"
#include "dp.h"

const rcc_cfg_t rcc_cfg = {
    .hse_val = 8000000,
    .pll_src = PLL_SRC_PREDIV,
    .pll_prediv = 1,
    .pll_mul = 6,
    .sysclk_src = SYSCLK_SRC_PLL,
    .hclk_div = HCLK_DIV1,
    .apb_div = {
        APB_DIV1,
    }
};

const pwm_cfg_t buz_cfg = {
    .ch = 1,
    .freq = 1000,
    .max_val = 200,
    .pclk = PCLK_TIM14,
    .tim = TIM14,
    .gpio = &(gpio_t){
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 4
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
            .af = 4
        }
    }
};

const usart_cfg_t debug_usart = {
    .usart = USART1,
    .default_baud = 115200,
    .rx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 10
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .pull = GPIO_PULL_NONE,
            .af = 1
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
            .af = 1
        }
    },
    .pclk = PCLK_USART1,
    .irqn = USART1_IRQn
};

const gpio_list_t buttons = {
    .count = 4,
    .cfg = {
        .mode = GPIO_MODE_INPUT,
        .pull = GPIO_PULL_UP,
    },
    .pin_list = (gpio_pin_t []){
        { .port = GPIO_PORT_B, .pin = 0 },      // LU
        { .port = GPIO_PORT_B, .pin = 1 },      // LD
        { .port = GPIO_PORT_B, .pin = 3 },      // RU
        { .port = GPIO_PORT_B, .pin = 4 },      // RD
    }
};

const usart_cfg_t kline_usart;

void kline_usart_irq_handler(void)
{
    usart_irq_handler(&kline_usart);
}

typedef union {
    struct {
        uint8_t byte;
        uint8_t flag;
    };
    uint16_t word;
} atomic_byte_flag_t;
atomic_byte_flag_t krx;

void kline_rx_byte_handler(char byte);

const usart_cfg_t kline_usart = {
    .usart = USART2,
    .default_baud = 9600,
    .rx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 3
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .pull = GPIO_PULL_UP,
            .af = 1,
        }
    },
    .tx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 2
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_OD,
            .af = 1,
        }
    },
    .tx_dma = {
        .dma_ch = 4,
    },
    // .rx_dma = {
    //     .dma_ch = 3,
    //     .size = 0,
    // },
    .pclk = PCLK_USART2,
    .irqn = USART2_IRQn,
    .usart_irq_handler = kline_usart_irq_handler,
    .rx_byte_handler = kline_rx_byte_handler
};

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&debug_usart, s, len);
}



uint8_t kline_rx_buf[32];
unsigned kline_rx_idx = 0;

mstimer_t kline_timeout = { .timeout = 50 };

void kline_rx_byte_handler(char byte)
{
    if(kline_rx_idx < 32) {
        kline_rx_buf[kline_rx_idx++] = byte;
    }
    mstimer_reset(&kline_timeout);
    dp("<- ");
    dpx(byte, 1);
    dn();
};

int main()
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t)PCLK_GPIOA, 1);
    pclk_ctrl(&(pclk_t)PCLK_GPIOB, 1);

    // init_gpio(&pwr_ctrl);
    // gpio_set_state(&pwr_ctrl, 0);

    init_systick();
    __enable_irq();

    init_pwm(&buz_cfg);
    pwm_set_freq(&buz_cfg, 700);
    pwm_set_ccr(&buz_cfg, 10);
    delay_ms(100);
    pwm_set_freq(&buz_cfg, 750);
    delay_ms(100);
    pwm_set_ccr(&buz_cfg, 0);

    usart_set_cfg(&debug_usart);
    usart_set_cfg(&kline_usart);

    dpn("Honda DLC emulator");

    while (1) {
        if (kline_rx_idx) {
            if (mstimer_is_over(&kline_timeout)) {
                kline_rx_idx = 0;
                dpn("timeout");
            }
        }
        if (kline_rx_idx == 5) {
            dp("req : ");
            dpxd(kline_rx_buf, 1, 5);
            dn();
            kline_rx_idx = 0;
        }
    };

    return 0;
}
