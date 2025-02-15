#include "config.h"

#include "systick.h"
#include "rtc.h"

#include "array_size.h"
#include "str_val.h"
#include "dp.h"

#include "mstimer.h"
#include "delay_blocking.h"


#include "dlc_poll.h"

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

    // gpio_cfg_t led_pin_cfg = {
    //     .mode = GPIO_MODE_OUTPUT,
    //     .speed = GPIO_SPEED_LOW,
    //     .type = GPIO_TYPE_PP,
    //     .pull = GPIO_PULL_NONE,
    // };

    // gpio_pin_t led_pin = {
    //     .port = GPIO_PORT_C,
    //     .pin = 13,
    // };

    // gpio_set_cfg(&led_pin, &led_pin_cfg);
    // gpio_set_state(&led_pin, 0);

    init_systick();
    __enable_irq();

    // for (unsigned i = 0; i < ARRAY_SIZE(debug_gpio_list); i++) {
    //     const gpio_cfg_t cfg = {
    //         .mode = GPIO_MODE_OUTPUT,
    //         .speed = GPIO_SPEED_HIGH,
    //         .type = GPIO_TYPE_PP,
    //         .pull = GPIO_PULL_NONE,
    //     };
    //     gpio_set_cfg(&debug_gpio_list[i], &cfg);
    //     gpio_set_state(&debug_gpio_list[i], 0);
    // }

    // for (unsigned i = 0; i < 4; i++) {
    //     gpio_set_state(&debug_gpio_list[0], 1);
    //     delay_ms(100);
    //     gpio_set_state(&debug_gpio_list[0], 0);
    //     delay_ms(100);
    // }

    usart_set_cfg(&debug_usart);
    usart_set_cfg(&kline_usart);

    dn();
    dpn("\n\n\nHONDA K-line trip computer");

    // // init_lcd_hw(&lcd_cfg);
    // // lcd_bl(4);

    // init_rtc();

    // unsigned rtc_last = 0;

    // mstimer_t led_flash_timer = mstimer_with_timeout(500);
    // unsigned led_state = 0;


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
