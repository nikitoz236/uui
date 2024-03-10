#include "mstimer.h"
#include "view.h"
#include "routes.h"

static mstimer_t screen_timeout = {};

#define SCREEN_ENGINE_OFF_TIMEOUT       10000
#define SCREEN_BUTTON_TIMEOUT           30000

static uint8_t engine_state = 0;
static uint8_t screen_state = 0;

void tc_routine(unsigned event)
{
    if (mstimer_is_over(&screen_timeout)) {
        if (engine_state == 0) {
            if (screen_state != 0) {
                screen_state = 0;
                view_screen_off();
            }
        }
    }
    if (event) {
        if (screen_state == 0) {
            screen_state = 1;
            view_screen_off();
        }
        if (engine_state == 0) {
            mstimer_start_timeout(&screen_timeout, SCREEN_BUTTON_TIMEOUT);
        }
    }
    view_process(event);
}

void tc_engine_set_status(unsigned state)
{
    if (state) {
        if (screen_state == 0) {
            screen_state = 1;
            view_screen_on();
        }
        route_trip_start();
    } else {
        mstimer_start_timeout(&screen_timeout, SCREEN_ENGINE_OFF_TIMEOUT);
        route_trip_end();
    }
    engine_state = state;
}
