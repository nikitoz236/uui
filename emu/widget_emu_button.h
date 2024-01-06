#pragma once
#include "widgets.h"

extern widget_t __widget_emu_button;

typedef struct __attribute__((packed)) {
    uint8_t state;
} widget_emu_button_ctx_t;

typedef	struct {
    char key;
    uint8_t index;
} __emu_button_index_cfg_t;

typedef	struct {
    xy_t size;
    unsigned color_released;
    unsigned color_pressed;
    unsigned color_text;
    __emu_button_index_cfg_t * index_cfg;
} widget_emu_button_cfg_t;
