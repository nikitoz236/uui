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
#include "sound_subsystem.h"

#include "time_subsystem.h"
#include "ui_subsystem.h"
#include "button_subsystem.h"


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

// sound_t sound_engine_run = {
sound_t sound_startup = {
    .notes = (struct sound_note []) {
        { .freq = 610, .ms_x10 = 10, .vol = 20 },
        { .freq = 650, .ms_x10 = 10, .vol = 20 },
        { .freq = 800, .ms_x10 = 15, .vol = 10 },
    },
    .n = 3
};

sound_t sound_btn_short = {
    .notes = (struct sound_note []) {
        { .freq = 800, .ms_x10 = 2, .vol = 20 },
    },
    .n = 1
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
    dn();
    storage_print_info();

    dn();
    init_date_time_subsystem();
    init_sound_subsystem(&buz_cfg);
    sound_play(&sound_startup);

    init_button_subsystem(&buttons);

    init_lcd_hw(&lcd_cfg);
    lcd_bl(4);
    init_lcd(&lcd_cfg);

    init_ui();

    while (1) {
        dlc_poll();
        sound_subsystem_process();
        storage_prepare_page();

        unsigned event = btn_get_event();
        if (event) {
            sound_play(&sound_btn_short);
            dp("button event: "); dpx(event, 1); dn();
        }
        ui_process(event);
    }

    return 0;
}
