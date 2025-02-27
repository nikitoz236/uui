#include "mstimer.h"
#include "gpio.h"

gpio_list_t * btns;
mstimer_t btn_dead_timer = { .timeout = 50 };

unsigned btn_last = 0;
unsigned btn_last_ret = 0;

void init_button_subsystem(gpio_list_t * cfg)
{
    btns = cfg;
    init_gpio_list(btns);
}

unsigned btn_get_event(void)
{
    unsigned btn_state = 0;
    // unsigned event = 0;
    for (unsigned i = 0; i < gpio_list_count(btns); i++) {
        if (gpio_list_get_state(btns, i)) {
            btn_state |= 1 << i;
            // event = i;
        }
    }

    if (btn_last != btn_state) {
        btn_last = btn_state;
        mstimer_reset(&btn_dead_timer);
    }

    if (mstimer_is_over(&btn_dead_timer)) {
        if (btn_last_ret != btn_last) {
            btn_last_ret = btn_last;
            return btn_last;
            // if (btn_last) {
            //     return event + 1;
            // }
        }
    }

    return 0;
}
