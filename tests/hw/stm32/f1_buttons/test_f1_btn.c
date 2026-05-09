#include "periph_header.h"
#include "periph_rcc.h"
#include "periph_gpio.h"
#include "periph_pclk.h"
#include "exti.h"
#include "stm_usart.h"
#include "systick.h"
#include "mstimer.h"
#include "time_units.h"

#define DP_NOTABLE
#include "dp.h"

const rcc_cfg_t rcc_cfg = {
    .hse_val = 8000000,
    .pll_src = PLL_SRC_PREDIV,
    .pll_prediv = 1,
    .pll_mul = 9,
    .sysclk_src = SYSCLK_SRC_PLL,
    .hclk_div = HCLK_DIV1,
    .apb_div = {
        APB_DIV2,
        APB_DIV1
    }
};

const usart_cfg_t debug_usart = {
    .usart = USART1,
    .default_baud = 115200,
    .tx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 9
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
        }
    },
    .pclk = PCLK_USART1,
};

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&debug_usart, s, len);
}


#include "btn_debounce.h"
#include "btn_press_processor.h"
#include "eq.h"

#define BTN_NUM 5

const gpio_list_t btn_inputs = {
    .count = BTN_NUM,
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
    },
};

static unsigned edge_counter[BTN_NUM] = {};

void on_exti_edge(unsigned idx, unsigned state)
{
    edge_counter[idx]++;
    btn_debounce_edge(idx, state);
}

// edges по тем же индексам что и pin_list в btn_inputs:
//   0 LU, 1 LD, 2 RU, 3 RM, 4 RD
static exti_group_t btn_exti_group = {
    .pins = &btn_inputs,
    .edges = (const exti_edge_t []){
        { .rise = 1, .fall = 1 },   // LU
        { .rise = 1, .fall = 1 },   // LD
        { .rise = 1, .fall = 1 },   // RU
        { .rise = 1, .fall = 1 },   // RM
        { .rise = 1, .fall = 1 },   // RD
    },
    .on_edge_idx = on_exti_edge,
};

void on_debounced(unsigned ch, unsigned state)
{
    dp("      debounce ch="); dpd(ch, 1); dp(" state="); dpd(state, 1);
    dp(" cnt="); dpd(edge_counter[ch], 3); dn();
    edge_counter[ch] = 0;
    btn_press_processor_edge(ch, state);
}

static const btn_debounce_cfg_t bdb =
    BTN_DEBOUNCE_INIT(BTN_NUM, on_debounced, 30);

void on_press_event(unsigned num, press_type_t type)
{
    const char * names[] = {
        "ON", "OFF", "SHORT", "LONG", "DOUBLE",
        "SHORT_LONG", "LONG_REPEAT", "SHORT_LONG_REPEAT",
    };
    dpd(get_uptime_ms(), 8); dp("  Key "); dpd(num, 2); dp(" "); dp(names[type]); dn();
}

static const btn_press_processor_cfg_t bpp =
    BTN_PRESS_PROCESSOR_INIT_FULL(BTN_NUM, on_press_event, 1000, 300, 200);

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t)PCLK_AFIO, 1);

    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    pclk_ctrl(&(pclk_t)PCLK_IOPA, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPB, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPC, 1);

    init_systick();
    usart_set_cfg(&debug_usart);
    init_gpio_list(&btn_inputs);
    exti_add_group(&btn_exti_group);
    init_btn_debounce(&bdb);
    init_btn_press_processor(&bpp);

    __enable_irq();

    dpn("Hey bitch ! this is keyboard test!");

    while (1) {
        task_process();
        while (eq_process()) {};
    };

    return 0;
}
