#pragma once

#include "esp32_gpio.h"
#include "esp32_spi.h"
#include "esp32_i2c.h"

#include "lcd_spi.h"
#include "aw9364.h"
#include "tca8418_kbd.h"
#include "dbg_usb_cdc_acm.h"

#define DP_NOTABLE
#include "dp.h"

extern spi_cfg_t spi;
extern const lcd_cfg_t lcd_cfg;
extern i2c_cfg_t i2c_bus_cfg;
extern gpio_t kbd_irq_line;

enum tlora_kbd {
    TLORA_KBD_KEY_Q,
    TLORA_KBD_KEY_W,
    TLORA_KBD_KEY_E,
    TLORA_KBD_KEY_R,
    TLORA_KBD_KEY_T,
    TLORA_KBD_KEY_Y,
    TLORA_KBD_KEY_U,
    TLORA_KBD_KEY_I,
    TLORA_KBD_KEY_O,
    TLORA_KBD_KEY_P,

    TLORA_KBD_KEY_A,
    TLORA_KBD_KEY_S,
    TLORA_KBD_KEY_D,
    TLORA_KBD_KEY_F,
    TLORA_KBD_KEY_G,
    TLORA_KBD_KEY_H,
    TLORA_KBD_KEY_J,
    TLORA_KBD_KEY_K,
    TLORA_KBD_KEY_L,
    TLORA_KBD_KEY_ENTER,

    TLORA_KBD_KEY_FN,
    TLORA_KBD_KEY_Z,
    TLORA_KBD_KEY_X,
    TLORA_KBD_KEY_C,
    TLORA_KBD_KEY_V,
    TLORA_KBD_KEY_B,
    TLORA_KBD_KEY_N,
    TLORA_KBD_KEY_M,
    TLORA_KBD_KEY_SHIFT,

    TLORA_KBD_KEY_BACKSPACE,
    TLORA_KBD_KEY_SPACE,
};
