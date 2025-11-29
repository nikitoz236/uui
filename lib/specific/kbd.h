#pragma once
#include <stdint.h>
#include "gpio.h"

typedef struct {
    gpio_list_t * rows;
    gpio_list_t * cols;
    uint8_t * prev_state;
    uint8_t row_dec : 1;
    uint8_t col_inv : 1;
} kbd_cfg_t;

void init_kbd(const kbd_cfg_t * cfg);
void kbd_scan(void (*change)(unsigned num, unsigned state));

