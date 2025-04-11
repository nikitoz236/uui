#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "periph_gpio.h"
#include "stm_usart.h"
#include "dp.h"

#include "systick.h"
#include "delay_blocking.h"
#include "array_size.h"

#include "adc_subsystem.h"

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

const usart_cfg_t debug_usart = {
    .usart = USART1,
    .default_baud = 115200,
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
    },
    .pclk = PCLK_USART1,
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

    init_adc();
    __enable_irq();

    dpn("Hey bitch ! this is honda tc adc test");

    uint16_t adc_result[2] = {};
    unsigned redraw = 1;

    while (1) {
        for (unsigned i = 0; i < 2; i++) {
            uint16_t v = adc_get(i);
            if (adc_result[i] != v) {
                redraw = 1;
            }
            adc_result[i] = v;
        }
        if (redraw) {
            dp("\r"); dpd(adc_result[0], 6); dp("  "); dpd(adc_result[1], 6);
        }
    };

    return 0;
}
