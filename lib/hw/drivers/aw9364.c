#include "aw9364.h"
#include "delay_blocking.h"

static gpio_t * pin;

void init_backlight(backlight_cfg_t * cfg)
{
    pin = &cfg->pin;
    init_gpio(pin);
}

void bl_set(unsigned lvl)
{
    if (lvl == 0 || lvl > 15) {
        gpio_set_state(pin, 0);
        return;
    }

    gpio_set_state(pin, 0);
    delay_ms(3);

    gpio_set_state(pin, 1);
    lvl--;

    while (lvl--) {
        delay_us(2);
        gpio_set_state(pin, 0);
        delay_us(2);
        gpio_set_state(pin, 1);
    }
}
