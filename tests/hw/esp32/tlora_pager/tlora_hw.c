#include "tlora_hw.h"

void __debug_usart_tx_data(const char * s, unsigned len)
{
    dbg_usb_cdc_acm_tx(s, len);
}

spi_cfg_t spi = {
    .spi = &GPSPI2,
    .pin_list = {
        [SPI_PIN_MOSI] = &(gpio_t){
            .cfg = { .mode = GPIO_MODE_SIG_IO },
            .pin = { .pin = 34, .signal = FSPID_OUT_IDX },
        },
        [SPI_PIN_SCK] = &(gpio_t){
            .cfg = { .mode = GPIO_MODE_SIG_IO },
            .pin = { .pin = 35, .signal = FSPICLK_OUT_IDX },
        },
        [SPI_PIN_MISO] = &(gpio_t){
            .cfg = { .mode = GPIO_MODE_SIG_IN },
            .pin = { .pin = 33, .signal = FSPIQ_IN_IDX },
        },
    }
};

const lcd_cfg_t lcd_cfg = {
    .bl = &(backlight_cfg_t){
        .pin = &(gpio_t){
            .cfg = { .mode = GPIO_MODE_OUT },
            .pin = { .pin = 42 }
        }
    },
    .no_reset = 1,
    .ctrl_lines = &(gpio_list_t){
        .cfg = { .mode = GPIO_MODE_OUT },
        .count = 1,
        .pin_list = {
            [LCD_DC] = { .pin = 37 }
        }
    },
    .spi_dev = {
        .cs_pin = &(gpio_t){
            .cfg = { .mode = GPIO_MODE_OUT },
            .pin = { .pin = 38 }
        },
        .spi = &spi,
    },
    .gcfg = {
        .height = 222,
        .width = 480,
        .x_offset = 0,
        .y_offset = 49,
        .x_flip = 1,
        .y_flip = 1,
        .bgr = 1
    }
};

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
