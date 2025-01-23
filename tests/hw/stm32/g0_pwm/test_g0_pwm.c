#include "periph_header.h"
#include "stm32g0x_rcc.h"
#include "stm32f0x_gpio.h"
#include "stm32g0x_pclk.h"
#include "irq_vectors.h"
#include "stm_pwm.h"

#include "stm_usart.h"
#include "dp.h"

const hw_rcc_cfg_t rcc_cfg = {
    .hse_val = 8000000,
    .pll_src = PLL_SRC_HSE,
    .pll_div = 1,
    .pll_mul = 16,
    .pll_sys_div = 2,
    .sysclk_src = SYSCLK_SRC_PLL,
    .hclk_div = HCLK_DIV1,
    .apb_div = {
        [PCLK_BUS_APB1] = APB_DIV1,
    }
};

const usart_cfg_t debug_usart = {
    .usart = USART1,
    .default_baud = 115200,
    .rx_pin = &(gpio_pin_cfg_t) {
        .gpio = {GPIO_PORT_A, 10},
        .cfg = {
            .mode = GPIO_MODE_AF,
            .pull = GPIO_PULL_NONE,
            .af = 1,
        }
    },
    .tx_pin = & (gpio_pin_cfg_t) {
        .gpio = {GPIO_PORT_A, 9},
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
            .af = 1,
        }
    },
    .pclk = PCLK_USART1,
    .irqn = USART1_IRQn
};

const usart_cfg_t usart_2 = {
    .usart = USART2,
    .pclk = PCLK_USART2,
    .irqn = USART2_IRQn,
    .default_baud = 115200,
    .tx_pin = & (gpio_pin_cfg_t) {
        .gpio = {GPIO_PORT_A, 2},
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
            .af = 1,
        }
    },
};

const pwm_cfg_t pwm_cfg = {
    .tim = TIM3,
    .tim_pclk = PCLK_TIM3,
    .gpio = &(gpio_pin_cfg_t){
        .gpio = {
            .port = GPIO_PORT_C,
            .pin = 7
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
            .af = 1,
        }
    },
    .ch = 2 - 1,
    .freq = 10000,
    .max_val = 10,
};

const gpio_pin_cfg_t pin_cfg = {
    .gpio = {
        .port = GPIO_PORT_A,
        .pin = 3
    },
    .cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_HIGH,
        .type = GPIO_TYPE_PP,
    }
};

unsigned lvl = 0;
unsigned state = 0;
void tim_irq_handler(void)
{
    TIM1->SR &= ~TIM_SR_UIF;
    state = !state;
    gpio_set_state(&pin_cfg.gpio, state);


    // lvl++;
    // if (lvl == 11) {
    //     lvl = 0;
    // }
    // pwm_set_ccr(&pwm_cfg, lvl);
}

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&debug_usart, s, len);
}

int main(void)
{
    pclk_ctrl(&PCLK_PWR, 1);

    pclk_ctrl(&PCLK_FLASH, 1);
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&PCLK_GPIOA, 1);
    pclk_ctrl(&PCLK_GPIOC, 1);


    usart_set_cfg(&usart_2);
    usart_set_cfg(&debug_usart);

    usart_tx(&usart_2, "Hello world!\n", 13);
    // usart_tx(&debug_usart, "Hello world!\n", 13);

    dn();
    dpn("hey!!!");
    dp("SYS CLOCK: ");
    dpd(pclk_f_hclk(), 10);
    dp("   FLASH ACR: ");
    dpx(FLASH->ACR, 4);
    dn();


    uint32_t * ptr = (uint32_t*)RCC;

    dpn("RCC:");
    for (unsigned i = 0; i < (sizeof(RCC_TypeDef) / sizeof(uint32_t)); i++) {
        dpx(*ptr, 4);
        dn();
        ptr++;
    }

    //==========================================================================

    gpio_configure(&pin_cfg);

    pclk_ctrl(&PCLK_TIM1, 1);

    TIM1->ARR = 500 - 1;
    TIM1->PSC = (pclk_f_timer(&PCLK_TIM1) / (1000 * 10)) - 1;
    TIM1->DIER |= TIM_DIER_UIE;

    NVIC_SetHandler(TIM1_BRK_UP_TRG_COM_IRQn, tim_irq_handler);
    NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);

    __enable_irq();

    TIM1->CR1 |= TIM_CR1_CEN;

    // gpio_set_state(&pin_cfg.gpio, 1);


    init_pwm(&pwm_cfg);
    pwm_set_ccr(&pwm_cfg, 1);

    // gpio_set_state(&pin_cfg.gpio, 0);

    while (1) {};

    return 0;
}
