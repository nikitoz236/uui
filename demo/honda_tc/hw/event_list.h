#pragma once

enum {
    EVENT_NONE      = 0,
    EVENT_BTN_BACK  = 1 << 0,
    EVENT_BTN_MODE  = 1 << 1,
    EVENT_BTN_UP    = 1 << 2,
    EVENT_BTN_OK    = 1 << 3,
    EVENT_BTN_DOWN  = 1 << 4,




    EVENT_BTN_LEFT = EVENT_BTN_BACK,
    EVENT_BTN_RIGHT,


    EVENT_LONG_BTN_LEFT,
    EVENT_LONG_BTN_RIGHT,
    EVENT_LONG_BTN_UP,
    EVENT_LONG_BTN_DOWN,
    EVENT_LONG_BTN_OK,

    EVENT_ENGINE_START,
    EVENT_ENGINE_STOP,
    EVENT_SLEEP,
    EVENT_WAKEUP,


};
