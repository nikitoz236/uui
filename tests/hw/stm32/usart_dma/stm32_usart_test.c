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
uint8_t rx_data[16] = {};
#include "stm_rcc_common.h"

int main(void)
{
    rcc_apply_cfg(&hw_rcc_cfg);

    pclk_ctrl(&PCLK_AFIO, 1);

    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
    AFIO->MAPR |= AFIO_MAPR_TIM1_REMAP_PARTIALREMAP;
    AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_PARTIALREMAP;

    pclk_ctrl(&PCLK_IOPA, 1);
    pclk_ctrl(&PCLK_IOPB, 1);
    pclk_ctrl(&PCLK_IOPC, 1);

    pclk_ctrl(&PCLK_DMA1, 1);

    // init_systick();
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

    // while (1) {};

    // delay_ms(100);
    // gpio_set_state(&debug_gpio_list[0], 0);

    usart_set_cfg(&debug_usart);

    dpn("Hey bitch!");
    dpn("this is stm32 usart test project!");

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



    // ну тут мы напарываемся на то что у нас dpxd странно работает, так как отправка дма без промежуточного буфера

    char ta[] = "012345";

    dp("test array str: "); dp(ta); dp(" dump: "); dpxd(ta, 1, 8); dn();


    while (1) {
        usart_rx(&debug_usart, rx_data, 6);
        dpn("recieve 6 bytes:");

        while (usart_is_rx_available(&debug_usart) == 0) {};

        dp("data recieved! : "), dpxd(rx_data, 1, 6); dn();
    }

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
