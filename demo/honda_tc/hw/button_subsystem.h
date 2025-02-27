#pragma once
#include "gpio.h"
#include "mstimer.h"

// typedef struct {
//     gpio_list_t * gpio_list;
//     mstimer_t * debounce_timer;
// } keypad_t;

/*
как это должно работать?

у тебя есть номер кнопки и событие которое с ней произошло
    нажали
    отпустили
    долго держат
    продолжают долго держать - инкременты



*/



void init_button_subsystem(gpio_list_t * cfg);
unsigned btn_get_event(void);
