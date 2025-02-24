#include "config.h"

#include "systick.h"
#include "rtc.h"

#include "array_size.h"
#include "str_val.h"
#include "dp.h"

#include "mstimer.h"
#include "delay_blocking.h"

#include "api_lcd_color.h"

#include "dlc_poll.h"
#include "date_time.h"

#include "storage.h"

// time_subsystem.c
void init_date_time_subsystem(void);

void metric_ecu_data_ready(unsigned addr, const uint8_t * data, unsigned len)
{
    dp("    -- metric_ecu_data_ready: ");
    dpx(addr, 1);
    dp(" len: ");
    dpd(len, 2);
    dp(" data: ");
    dpxd(data, 1, len);
    dn();
}

void tc_engine_set_status(unsigned state)
{
    dp("    -- tc_engine_set_status: ");
    dpd(state, 1);
    dn();
}

const gpio_list_t debug_gpio_list = {
    .count = 3,
    .cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_HIGH,
        .type = GPIO_TYPE_PP,
    },
    .pin_list = (gpio_pin_t []){
        { .port = GPIO_PORT_C, .pin = 13 },
        { .port = GPIO_PORT_B, .pin = 8 },
        { .port = GPIO_PORT_B, .pin = 9 }
    }
};

const gpio_list_t buttons = {
    .count = 5,
    .cfg = {
        .mode = GPIO_MODE_INPUT,
        .pull = GPIO_PULL_NONE,
    },
    .pin_list = (gpio_pin_t []){
        { .port = GPIO_PORT_B, .pin = 3 },      // LU
        { .port = GPIO_PORT_A, .pin = 15 },     // LD
        { .port = GPIO_PORT_B, .pin = 4 },      // RU
        { .port = GPIO_PORT_B, .pin = 6 },      // RM
        { .port = GPIO_PORT_B, .pin = 7 },      // RD
    }
};

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

    init_gpio_list(&buttons);
    init_gpio_list(&debug_gpio_list);

    init_systick();
    __enable_irq();

    usart_set_cfg(&debug_usart);
    usart_set_cfg(&kline_usart);

    dn();
    dn();
    dn();
    dpn("HONDA K-line trip computer");

    storage_init();
    storage_print_info();

    dn();
    init_date_time_subsystem();

    init_lcd_hw(&lcd_cfg);
    lcd_bl(4);
    init_lcd(&lcd_cfg);

    init_pwm(&buz_cfg);

    pwm_set_freq(&buz_cfg, 600);
    pwm_set_ccr(&buz_cfg, 10);
    delay_ms(100);
    pwm_set_freq(&buz_cfg, 800);
    delay_ms(100);
    pwm_set_ccr(&buz_cfg, 0);

    lcd_rect(10, 20, 30, 40, 0xA14C);


    while (1) {
        dlc_poll();
        // unsigned rtc_s = rtc_get_time_s();
        // if (rtc_last != rtc_s) {
        //     rtc_last = rtc_s;
        //     dp("rtc time: ");
        //     dpd(rtc_s, 10);
        //     dn();
        // }


        // if (mstimer_do_period(&led_flash_timer)) {
        //     led_state++;
        //     led_state &= 1;
        //     gpio_set_state(&led_pin, led_state);
        // }
    }

    return 0;
}
