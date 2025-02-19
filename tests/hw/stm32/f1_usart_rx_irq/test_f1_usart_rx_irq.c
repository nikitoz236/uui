#include "periph_header.h"
#include "periph_rcc.h"
#include "periph_gpio.h"
#include "periph_pclk.h"
#include "systick.h"
#include "stm_usart.h"
#include "dp.h"

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

volatile unsigned flag = 0;
char rx_byte = 0;

void debug_usart_rx_byte_handler(char c)
{
    rx_byte = c;
    flag = 1;
}

const usart_cfg_t debug_usart;

void debug_usart_irq_handler(void)
{
    usart_irq_handler(&debug_usart);
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
    .rx_byte_handler = debug_usart_rx_byte_handler,
    .usart_irq_handler = debug_usart_irq_handler,
    .pclk = PCLK_USART1,
    .irqn = USART1_IRQn
};

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&debug_usart, s, len);
}

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t)PCLK_AFIO, 1);

    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    pclk_ctrl(&(pclk_t)PCLK_IOPA, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPB, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPC, 1);

    pclk_ctrl(&(pclk_t)PCLK_DMA1, 1);
    init_systick();

    usart_set_cfg(&debug_usart);

    __enable_irq();

    dpn("Hey bitch ! this is usart irq test!");

    while (1) {
        if (flag) {
            flag = 0;
            dp("recieved: "); dpl(&rx_byte, 1); dn();
        }
    };

    return 0;
}
