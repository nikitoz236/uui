#pragma once

#include "btn_list.h"

enum {
    EVENT_NONE = 0,
    EVENT_BTN_BACK = (1 << BTN_LU),
    EVENT_BTN_UP = (1 << BTN_RU),
    EVENT_BTN_DOWN = (1 << BTN_RD),
    EVENT_BTN_OK = (1 << BTN_OK),
    EVENT_BTN_LEFT = EVENT_BTN_BACK,
    EVENT_BTN_MOD,
};
