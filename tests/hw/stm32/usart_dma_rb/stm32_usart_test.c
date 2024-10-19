#include "config.h"
#include "dp.h"
#include "array_size.h"
#include "systick.h"
#include "mstimer.h"
#include "delay_blocking.h"

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&debug_usart, s, len);
}

/*
    конфигурация логанализатора
    CH1 - USART_TX  PA9
    CH2 - LED       PC13
    CH3 - CAN_RX    PB8
    CH4 - CAN_TX    PB9
    CH5 - LCD_RST   PA12

    CH7 - LCD_DC    PA11
 */

const gpio_pin_t debug_gpio_list[] = {
    { GPIO_PORT_C, 13 },
    { GPIO_PORT_B, 8 },
    { GPIO_PORT_B, 9 }
};

const char wf[] = {"Dark spruce forest frowned on either side the frozen waterway. The trees had been stripped by a recent wind of their white covering of frost, and they seemed to lean towards each other, black and ominous, in the fading light."};

int main(void)
{
    //  000 Zero wait state, if 0 < SYSCLK≤ 24 MHz
    //  001 One wait state, if 24 MHz < SYSCLK ≤ 48 MHz
    //  010 Two wait states, if 48 MHz < SYSCLK ≤ 72 MHz
    FLASH->ACR |= FLASH_ACR_LATENCY * 2;
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    hw_rcc_apply_cfg(&hw_rcc_cfg);

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
    AFIO->MAPR |= AFIO_MAPR_TIM1_REMAP_PARTIALREMAP;
    AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_PARTIALREMAP;

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    const hw_pclk_t dma_pclk = {
        .mask = RCC_AHBENR_DMA1EN,
        .bus = PCLK_BUS_AHB
    };

    hw_rcc_pclk_ctrl(&dma_pclk, 1);

    init_systick();
    __enable_irq();

    for (unsigned i = 0; i < ARRAY_SIZE(debug_gpio_list); i++) {
        const gpio_cfg_t cfg = {
            .mode = GPIO_MODE_OUTPUT,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
            .pull = GPIO_PULL_NONE,
        };
        gpio_set_cfg(&debug_gpio_list[i], &cfg);
        gpio_set_state(&debug_gpio_list[i], 0);
    }

    gpio_set_state(&debug_gpio_list[0], 1);
    delay_ms(100);
    gpio_set_state(&debug_gpio_list[0], 0);

    usart_set_cfg(&debug_usart);

    dpn("Hey bitch!");

    dpn("this is stm32 usart test project!");
    dpn("Another str");
    //-->                                                                <- 64 spaces

    gpio_set_state(&debug_gpio_list[1], 1);

    dpn(wf);

    gpio_set_state(&debug_gpio_list[1], 0);

    dp("test hex dump: wf ptr: "); dpx((unsigned)wf, 4); dp(" ptr of next char: "); dpx((unsigned)(&wf[1]), 4); dp(" dump: "); dpxd(wf, 1, 16); dn();

    while (1) {};

    return 0;
}


/*
видим в терминале:

Hey bitch!
this is stm32 usart test project!
Another str
Dark spruce forest frowned on either side the frozen waterway. The trees had been stripped by a recent wind of their white covering of frost, and they seemed to lean towards each other, black and ominous, in the fading light.
test hex dump: wf ptr: 08000964 ptr of next char: 08000965 dump: 44 61 72 6B 20 73 70 72 75 63 65 20 66 6F 72 65

*/
