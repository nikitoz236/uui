#include "periph_header.h"
#include "periph_rcc.h"
#include "periph_gpio.h"
#include "periph_pclk.h"
#include "stm_usart.h"
#include "systick.h"
#include "mstimer.h"
#include "time_units.h"
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
    .pclk = PCLK_USART1,
};

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&debug_usart, s, len);
}

mstimer_t cyc_period = { .timeout = 1000, .start = 0 };

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t)PCLK_AFIO, 1);

    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    pclk_ctrl(&(pclk_t)PCLK_IOPA, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPB, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPC, 1);

    init_systick();
    usart_set_cfg(&debug_usart);

    __enable_irq();

    dpn("Hey bitch ! this is usart irq test!");

    dp("System debug: "); dpxd(CoreDebug, 4, sizeof(CoreDebug_Type) / 4); dn();
    dp("ITM_TCR: "); dpx(&ITM->TCR, 4); dn();
    dp("ITM_TCR_DWTENA: "); dpd((ITM->TCR & ITM_TCR_DWTENA_Msk), 4); dn();

    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    #define    DWT_CYCCNT    *(volatile uint32_t*)0xE0001004

    DWT_CYCCNT = 0;

    while (1) {
        if (mstimer_do_period(&cyc_period)) {
            dp("sec : "); dpd(uptime_ms / MS_IN_S, 10); dp("  DWT: "); dpxd(DWT_BASE, 4, 4); dn();

        }
    };

    return 0;
}
