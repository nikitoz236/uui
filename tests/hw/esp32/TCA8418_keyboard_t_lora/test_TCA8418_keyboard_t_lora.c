#include "dbg_usb_cdc_acm.h"

#define DP_NOTABLE
#include "dp.h"

#include "delay_blocking.h"
#include "esp32_gpio.h"
#include "esp32_i2c.h"

void __debug_usart_tx_data(const char * s, unsigned len)
{
    dbg_usb_cdc_acm_tx(s, len);
}

i2c_cfg_t i2c_bus_cfg = {
    .dev = &I2C0,
    .i2c_pclk = SYSTEM_I2C_EXT0_CLK_EN_S,
    .freq = 400000,
    .pin_list = &(gpio_list_t){
        .count = 2,
        .cfg = {
            .mode = GPIO_MODE_SIG_IO,
            .pu = 1,
            .od = 1
        },
        .pin_list = {
            { .pin = 2, .signal = I2CEXT0_SCL_IN_IDX },
            { .pin = 3, .signal = I2CEXT0_SDA_IN_IDX }
        }
    }
};

gpio_t kbd_irq_line = {
    .cfg = { .mode = GPIO_MODE_IN, .pu = 1 },
    .pin = { .pin = 6 }
};

#define I2C_TCA8418_ADDR 0x34

uint8_t dump[0x30] = {};

enum tca8418_regs {
    TCA8418_REG_CFG = 1,
    TCA8418_REG_INT_STAT,
    TCA8418_REG_KEY_LCK_EC,
    TCA8418_REG_KEY_EVENT_A,
    TCA8418_REG_KEY_EVENT_B,
    TCA8418_REG_KEY_EVENT_C,
    TCA8418_REG_KEY_EVENT_D,
    TCA8418_REG_KEY_EVENT_E,
    TCA8418_REG_KEY_EVENT_F,
    TCA8418_REG_KEY_EVENT_G,
    TCA8418_REG_KEY_EVENT_H,
    TCA8418_REG_KEY_EVENT_I,
    TCA8418_REG_KEY_EVENT_J,
    TCA8418_REG_KP_LCK_TIMER,
    TCA8418_REG_UNLOCK1,
    TCA8418_REG_UNLOCK2,
    TCA8418_REG_GPIO_INT_STAT1,
    TCA8418_REG_GPIO_INT_STAT2,
    TCA8418_REG_GPIO_INT_STAT3,
    TCA8418_REG_GPIO_DAT_STAT1,
    TCA8418_REG_GPIO_DAT_STAT2,
    TCA8418_REG_GPIO_DAT_STAT3,
    TCA8418_REG_GPIO_DAT_OUT1,
    TCA8418_REG_GPIO_DAT_OUT2,
    TCA8418_REG_GPIO_DAT_OUT3,
    TCA8418_REG_GPIO_INT_EN1,
    TCA8418_REG_GPIO_INT_EN2,
    TCA8418_REG_GPIO_INT_EN3,
    TCA8418_REG_KP_GPIO1,
    TCA8418_REG_KP_GPIO2,
    TCA8418_REG_KP_GPIO3,
    TCA8418_REG_GPI_EM1,
    TCA8418_REG_GPI_EM2,
    TCA8418_REG_GPI_EM3,
    TCA8418_REG_GPIO_DIR1,
    TCA8418_REG_GPIO_DIR2,
    TCA8418_REG_GPIO_DIR3,
    TCA8418_REG_GPIO_INT_LVL1,
    TCA8418_REG_GPIO_INT_LVL2,
    TCA8418_REG_GPIO_INT_LVL3,
    TCA8418_REG_DEBOUNCE_DIS1,
    TCA8418_REG_DEBOUNCE_DIS2,
    TCA8418_REG_DEBOUNCE_DIS3,
    TCA8418_REG_GPIO_PULL1,
    TCA8418_REG_GPIO_PULL2,
    TCA8418_REG_GPIO_PULL3,

    TCA8418_REG_KEY_EVENT_START = TCA8418_REG_KEY_EVENT_A,
    TCA8418_REG_KEY_EVENT_LEN = TCA8418_REG_KEY_EVENT_J - TCA8418_REG_KEY_EVENT_A + 1,
};

void init_tca8418(void)
{
    const uint8_t cmd_cfg[] = {TCA8418_REG_CFG, 0x81 };
    i2c_transaction(I2C_TCA8418_ADDR, cmd_cfg, sizeof(cmd_cfg), 0, 0);

    const uint8_t cmd_kp_en[] = { TCA8418_REG_KP_GPIO1, 0xFF, 0xFF, 0x03 };
    i2c_transaction(I2C_TCA8418_ADDR, cmd_kp_en, sizeof(cmd_kp_en), 0, 0);

    // const uint8_t cmd_cfg[] = {TCA8418_REG_CFG, 0x80 };

}

void tca8418_poll_kp_fifo(void (*handler)(unsigned idx, unsigned state))
{
    uint8_t key_events[TCA8418_REG_KEY_EVENT_LEN];
    uint8_t reg = TCA8418_REG_KEY_EVENT_START;
    i2c_transaction(I2C_TCA8418_ADDR, &reg, 1, key_events, TCA8418_REG_KEY_EVENT_LEN);
    while (i2c_status == I2C_STATUS_BUSY) {};
    for (unsigned i = 0; i < TCA8418_REG_KEY_EVENT_LEN; i++) {
        uint8_t event = key_events[i];
        if (event) {
            unsigned state = 0;
            if (event & 0x80) {
                state = 1;
            }
            unsigned idx = event & 0x7F;
            handler(idx, state);
        } else {
            break;
        }
    }
}


/*

                                1E
    01 02 03 04 05 06 07 08 09 0A
    0B 0C 0D 0E 0F 10 11 12 13 14
    15 16 17 18 19 1A 1B 1C 1D
                  1F

*/

void kbd_change_handler(unsigned num, unsigned state)
{
    dp("key : "); dpd(num, 2); dp(" = "); dpd(state, 1); dn();
    // if (state) {
    //     uint8_t c = sym[num];
    //     if (c) {
    //         __debug_usart_tx_data(&c, 1);
    //     }
    // }
}


int main(void)
{
    dpn("TCA8418 t lora keyboard");

    init_gpio(&kbd_irq_line);
    init_i2c(&i2c_bus_cfg);
    init_tca8418();

    while (1) {
        tca8418_poll_kp_fifo(kbd_change_handler);
        // unsigned int_stat = gpio_get_state(&kbd_irq_line);
        // uint8_t key_events[TCA8418_REG_KEY_EVENT_LEN];
        // uint8_t reg = TCA8418_REG_KEY_EVENT_START;
        // i2c_transaction(I2C_TCA8418_ADDR, &reg, 1, key_events, TCA8418_REG_KEY_EVENT_LEN);

        // dp("keyboard poll : irq["); dpd(int_stat, 1); dp("] event buf: "); dpxd(key_events, 1, TCA8418_REG_KEY_EVENT_LEN); dn();
        delay_ms(100);
    };
}


