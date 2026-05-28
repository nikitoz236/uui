#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "periph_gpio.h"
#include "periph_i2c.h"

#include "stm_usart.h"
#include "systick.h"
#include "uptime.h"
#include "timers_32.h"

#include "ina3221.h"

#include "dp.h"


#define SHUNT_MILLIOHM   100
#define PRINT_PERIOD_MS  1000


const rcc_cfg_t rcc_cfg = {
    .hse_val = 8000000,
    .pll_src = PLL_SRC_PREDIV,
    .pll_prediv = 1,
    .pll_mul = 6,
    .sysclk_src = SYSCLK_SRC_PLL,
    .hclk_div = HCLK_DIV1,
    .apb_div = {
        APB_DIV1,
    }
};


const usart_cfg_t debug_usart;

void debug_usart_irq_handler(void)
{
    usart_irq_handler(&debug_usart);
}

const usart_cfg_t debug_usart = {
    .usart = USART1,
    .default_baud = 115200,
    .rx_pin = &(const gpio_t){
        .gpio = { .port = GPIO_PORT_A, .pin = 10 },
        .cfg = { .mode = GPIO_MODE_AF, .pull = GPIO_PULL_NONE, .af = 1 },
    },
    .tx_pin = &(const gpio_t){
        .gpio = { .port = GPIO_PORT_A, .pin = 9 },
        .cfg = { .mode = GPIO_MODE_AF, .speed = GPIO_SPEED_HIGH, .type = GPIO_TYPE_PP, .af = 1 },
    },
    .tx_dma = {
        .dma_ch = 2,
    },
    .pclk = PCLK_USART1,
};

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&debug_usart, s, len);
}


const i2c_cfg_t i2c_bus = {
    .pins = &(gpio_list_t){
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_OD,
            .pull = GPIO_PULL_UP,
            .af = 1,
        },
        .count = 2,
        .pin_list = (gpio_pin_t[]){
            { .port = GPIO_PORT_B, .pin = 6 },
            { .port = GPIO_PORT_B, .pin = 7 },
        },
    },
    .i2c = I2C1,
    .irqn = I2C1_IRQn,
    .pclk = PCLK_I2C1,
    .clk_src = I2C_CLK_SYSCLK,
    I2C_TIMING_100K(48000000),
};


const ina3221_cfg_t ina_cfg = {
    .ch1_en  = 1,
    .ch2_en  = 1,
    .ch3_en  = 1,
    .avg     = INA3221_AVG_64,
    .vbus_ct = INA3221_CT_1100US,
    .vsh_ct  = INA3221_CT_1100US,
    .mode    = INA3221_MODE_SHUNT_BUS_CONT,
};


void print_channel(unsigned ch)
{
    int32_t v_mv  = ina3221_bus_mv(ch);
    int32_t sh_uv = ina3221_shunt_uv(ch);
    int32_t i_ua  = ina3221_uv_to_ua(sh_uv, SHUNT_MILLIOHM);

    dp(" | ");
    dpds(v_mv, 6);
    dp(" ");
    dpds(sh_uv, 7);
    dp(" ");
    dpds(i_ua, 7);
}


int main(void)
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t)PCLK_GPIOA, 1);
    pclk_ctrl(&(pclk_t)PCLK_GPIOB, 1);
    pclk_ctrl(&(pclk_t)PCLK_DMA, 1);

    usart_set_cfg(&debug_usart);

    init_systick();
    __enable_irq();

    init_i2c(&i2c_bus);

    dpn("STM32F0 INA3221 test");
    dp("manuf id: ");
    dpx(ina3221_manuf_id(), 2);
    dn();
    dp("die id:   ");
    dpx(ina3221_die_id(), 2);
    dn();

    ina3221_init(&ina_cfg);

    timer_32_t tmr = {};
    t32_run(&tmr, get_uptime_ms(), PRINT_PERIOD_MS);

    dpn("        ch1                  |        ch2                  |        ch3");
    dpn("  V[mV] Vsh[uV]   I[uA]      |  V[mV] Vsh[uV]   I[uA]      |  V[mV] Vsh[uV]   I[uA]");

    while (1) {
        if (t32_is_over(&tmr, get_uptime_ms())) {
            t32_run(&tmr, get_uptime_ms(), PRINT_PERIOD_MS);
            print_channel(0);
            print_channel(1);
            print_channel(2);
            dn();
        }
    };

    return 0;
}
