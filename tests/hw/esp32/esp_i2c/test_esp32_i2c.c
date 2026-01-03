

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

void i2c_scan(void)
{
    dpn("    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
    dp( "00:    ");

    for (uint8_t i = 1; i < 128; i++) {
        if ((i & 0x0F) == 0) {
            dn();
            dpx(i, 1);
            dp(": ");
        }
        i2c_transaction(i, 0, 0, 0, 0);
        while (i2c_status() == I2C_STATUS_BUSY) {};
        if (i2c_status() == I2C_STATUS_NACK) {
            dp("-- ");
        } else {
            dpx(i, 1);
            dp(" ");
        }
    }
    dn();
}

int main(void)
{
    dpn("tlora, ok");

    init_i2c(&i2c_bus_cfg);
    dpn("scan i2c bus");
    i2c_scan();
    dpn("done");

    while (1) {};
}
