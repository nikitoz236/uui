#include "config.h"
#include "str_val.h"
#include "dp.h"
#include "array_size.h"
#include "systick.h"
#include "mstimer.h"
#include "delay_blocking.h"

#include "dma.h"
#include "stddef.h"

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
    CH5 - LCD_DC    PA11
    CH6 - LCD_SCL   PA5
    CH7 - LCD_SDO   PA6
    CH8 - LCD_SDI   PA7
*/

typedef struct {
    uint8_t cmd;
    uint8_t frame_len;
    uint8_t offset;
    uint8_t len;
    uint8_t cs;
} kline_request_t;

static kline_request_t kline_request = {};

static uint8_t calc_cs(uint8_t * data, unsigned len)
{
    uint8_t cs = 0;
    while (len--) {
        cs -= *data++;
    }
    return cs;
}

void dump_unit(uint8_t unit_addr, uint8_t frames)
{
    mstimer_t timeout_timer = mstimer_with_timeout(250);

    uint8_t dlc_rx[32];
    for (uint8_t i = 0; i < frames; i++) {
        kline_request.cmd = unit_addr;
        kline_request.frame_len = sizeof(kline_request);
        kline_request.offset = i * 16;
        kline_request.len = 16;
        kline_request.cs = calc_cs((uint8_t *)&kline_request, offsetof(kline_request_t, cs));

        unsigned recieved = 16 + 3 + 5;
        usart_rx(&kline_usart, dlc_rx, recieved);
        mstimer_reset(&timeout_timer);
        usart_tx(&kline_usart, &kline_request, sizeof(kline_request));

        while (usart_is_rx_available(&kline_usart) == 0) {
            if (mstimer_is_over(&timeout_timer)) {
                break;
            }
        }

        unsigned remain = dma_get_cnt(kline_usart.rx_dma.dma_ch);

        dpx(i * 16, 1); dp(": ");
        dpxd(dlc_rx, 1, recieved - remain);
        if (remain) {
            dp(" !!! Timeout");
        }
        dn();
    }
}

void dumper(void)
{
    dpn("Honda test DLC dumper");
    while (1) {
        char cmd;
        usart_rx(&debug_usart, &cmd, 1);

        dn(); dpn("select unit to dump: E or S or A");

        while (usart_is_rx_available(&debug_usart) == 0) {};


        switch (cmd) {
        case 'E':
            dpn("ECU dump:");
            dump_unit(0x20, 16);
            break;
        case 'A':
            dpn("ABS dump:");
            dump_unit(0xA0, 8);
            break;
        case 'S':
            dpn("SRS dump:");
            dump_unit(0x60, 8);
            break;
        default:
            dpn("Unknown command");
            break;
        }
    }
}

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

    gpio_cfg_t led_pin_cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_LOW,
        .type = GPIO_TYPE_PP,
        .pull = GPIO_PULL_NONE,
    };

    gpio_pin_t led_pin = {
        .port = GPIO_PORT_C,
        .pin = 13,
    };

    const hw_pclk_t dma_pclk = {
        .mask = RCC_AHBENR_DMA1EN,
        .bus = PCLK_BUS_AHB
    };

    hw_rcc_pclk_ctrl(&dma_pclk, 1);

    gpio_set_cfg(&led_pin, &led_pin_cfg);
    gpio_set_state(&led_pin, 1);

    usart_set_cfg(&debug_usart);
    usart_set_cfg(&kline_usart);

    init_systick();
    __enable_irq();

    dumper();

    while (1) {};

    return 0;
}
