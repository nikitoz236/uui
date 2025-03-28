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

const gpio_t led = {
    .gpio = {
        .port = GPIO_PORT_C,
        .pin = 13
    },
    .cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_HIGH,
        .type = GPIO_TYPE_PP
    }
};

const char wf[] = {"Dark spruce forest frowned on either side the frozen waterway. The trees had been stripped by a recent wind of their white covering of frost, and they seemed to lean towards each other, black and ominous, in the fading light."};
uint8_t rx_data[16] = {};

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t)PCLK_AFIO, 1);

    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    pclk_ctrl(&(pclk_t)PCLK_IOPA, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPB, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPC, 1);

    pclk_ctrl(&(pclk_t)PCLK_DMA1, 1);

    // init_systick();
    __enable_irq();

    // init_gpio(&led);
    // gpio_set_state(&led, 1);

    // init_gpio(&debug_usart.rx_pin);
    // init_gpio(&debug_usart.tx_pin);

    init_gpio_list(&debug_gpio_list);
    gpio_list_set_state(&debug_gpio_list, 0, 1);

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

    dp("size of gpio_t: "); dpd(sizeof(gpio_t), 4); dn();

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
