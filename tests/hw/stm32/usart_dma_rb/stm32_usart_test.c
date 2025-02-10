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

const gpio_list_t debug_gpio_list = {
    .cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_HIGH,
        .type = GPIO_TYPE_PP,
    },
    .count = 3,
    .pin_list = (gpio_pin_t[]){
        { GPIO_PORT_C, 13 },
        { GPIO_PORT_B, 8 },
        { GPIO_PORT_B, 9 }
    }
};

const char wf[] = {"Dark spruce forest frowned on either side the frozen waterway. The trees had been stripped by a recent wind of their white covering of frost, and they seemed to lean towards each other, black and ominous, in the fading light."};

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t)PCLK_AFIO, 1);

    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
    AFIO->MAPR |= AFIO_MAPR_TIM1_REMAP_PARTIALREMAP;
    AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_PARTIALREMAP;

    pclk_ctrl(&(pclk_t)PCLK_IOPA, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPB, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPC, 1);

    pclk_ctrl(&(pclk_t)PCLK_DMA1, 1);

    init_systick();
    __enable_irq();

    init_gpio_list(&debug_gpio_list);

    gpio_list_set_state(&debug_gpio_list, 0, 1);
    delay_ms(200);
    gpio_list_set_state(&debug_gpio_list, 0, 0);

    usart_set_cfg(&debug_usart);

    dpn("Hey bitch!");

    dpn("this is stm32 usart test project!");
    dpn("Another str");
    //-->                                                                <- 64 spaces

    gpio_list_set_state(&debug_gpio_list, 1, 1);
    dpn(wf);
    gpio_list_set_state(&debug_gpio_list, 1, 0);

    dp("test hex dump: wf ptr: "); dpx((unsigned)wf, 4); dp(" ptr of next char: "); dpx((unsigned)(&wf[1]), 4); dp(" dump: "); dpxd(wf, 1, 16); dn();

    mstimer_t print_timer = mstimer_with_timeout(100);
    unsigned counter = 0;

    while (1) {
        if (mstimer_do_period(&print_timer)) {
            dp("counter: ");
            dpd(counter, 10);
            dn();
            counter++;
        }
    };

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
