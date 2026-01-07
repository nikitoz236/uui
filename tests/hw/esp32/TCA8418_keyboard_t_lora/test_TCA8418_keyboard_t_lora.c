#include "dbg_usb_cdc_acm.h"

#define DP_NOTABLE
#include "dp.h"

#include "delay_blocking.h"
#include "esp32_gpio.h"
#include "esp32_i2c.h"

#include "tca8418_kbd.h"

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
        if (gpio_get_state(&kbd_irq_line) == 0) {
            dpn("kbd irq detect");
            tca8418_poll_kp_fifo(kbd_change_handler);
        }
        // unsigned int_stat = gpio_get_state(&kbd_irq_line);
        // uint8_t key_events[TCA8418_REG_KEY_EVENT_LEN];
        // uint8_t reg = TCA8418_REG_KEY_EVENT_START;
        // i2c_transaction(I2C_TCA8418_ADDR, &reg, 1, key_events, TCA8418_REG_KEY_EVENT_LEN);

        // const uint8_t cmd_clen_irq[] = { TCA8418_REG_INT_STAT, 1 };
        // i2c_transaction(I2C_TCA8418_ADDR, cmd_clen_irq, sizeof(cmd_clen_irq), 0, 0);

        // dp("keyboard poll : irq["); dpd(int_stat, 1); dp("] event buf: "); dpxd(key_events, 1, TCA8418_REG_KEY_EVENT_LEN); dn();
        delay_ms(100);
    };
}


