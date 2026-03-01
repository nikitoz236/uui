#include "aw9364.h"
#include "delay_blocking.h"

void init_backlight(backlight_cfg_t * cfg)
{
    init_gpio(cfg->pin);
}

void bl_set(backlight_cfg_t * cfg, unsigned lvl)
{
    gpio_t * pin = cfg->pin;

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
