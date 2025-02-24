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

// sound_t sound_engine_run = {
sound_t sound_startup = {
    .notes = (struct sound_note []) {
        { .freq = 610, .ms_x10 = 10, .vol = 20 },
        { .freq = 650, .ms_x10 = 10, .vol = 20 },
        { .freq = 800, .ms_x10 = 15, .vol = 10 },
    },
    .n = 3
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
    dn();
    storage_print_info();

    dn();
    init_date_time_subsystem();
    init_sound_subsystem(&buz_cfg);
    sound_play(&sound_startup);

    init_lcd_hw(&lcd_cfg);
    lcd_bl(4);
    init_lcd(&lcd_cfg);

    lcd_rect(10, 20, 30, 40, 0xA14C);

    while (1) {
        dlc_poll();
        sound_subsystem_process();
        storage_prepare_page();
    }

    return 0;
}
