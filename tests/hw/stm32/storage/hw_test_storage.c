#include "config.h"

#include "dp.h"
#include "storage.h"

/*
    конфигурация логанализатора
    CH1 - USART_TX  PA9
    CH2 - LED       PC13
    CH3 - CAN_RX    PB8
    CH4 - CAN_TX    PB9
    CH5 - LCD_DC    PA11
    CH6 - LCD_SCL   PA5
    CH7 - LCD_SDO   PA6
    CH8 - LCD_SDI   PA7

 */

const gpio_pin_t debug_gpio_list[] = {
    { GPIO_PORT_C, 13 },
    { GPIO_PORT_B, 8 },
    { GPIO_PORT_B, 9 }
};

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

    usart_set_cfg(&debug_usart);
    __enable_irq();

    dn(); dn(); dpn("TEST STORAGE");

    storage_init();
    storage_print_info();

    while (1) {
    };

    return 0;
}
