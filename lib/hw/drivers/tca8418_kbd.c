#include "i2c.h"

#define I2C_TCA8418_ADDR 0x34

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
}

void tca8418_poll_kp_fifo(void (*handler)(unsigned idx, unsigned state))
{
    uint8_t key_events[TCA8418_REG_KEY_EVENT_LEN];

    uint8_t reg = TCA8418_REG_KEY_EVENT_START;
    i2c_transaction(I2C_TCA8418_ADDR, &reg, 1, key_events, TCA8418_REG_KEY_EVENT_LEN);

    const uint8_t cmd_clen_irq[] = { TCA8418_REG_INT_STAT, 1 };
    i2c_transaction(I2C_TCA8418_ADDR, cmd_clen_irq, sizeof(cmd_clen_irq), 0, 0);

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
