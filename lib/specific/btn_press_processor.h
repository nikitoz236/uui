#pragma once
#include <stdint.h>

/*
    обработчик нажатий поверх debounced edge

    память: 2 битмапа input_state и long_pressed (по DIV_ROUND_UP(num,8) байт),
            +1 битмап double_pressed если task_double задан.
            task_long обязателен, task_double=NULL отключает DOUBLE/SHORTLONG.
            repeat_ms=0 отключает повторы.

    инициализация - макрос разворачивается в { ... } инициализатор:
        static const btn_press_processor_cfg_t my =
            BTN_PRESS_PROCESSOR_INIT_SIMPLE(8, my_handler, 1000);
*/

#include "tasks.h"
#include "round_up.h"

typedef enum {
    PRESS_ON,                   // сразу на edge press
    PRESS_OFF,                  // сразу на edge release
    PRESS_SHORT,                // release раньше long_ms (через double_ms если есть)
    PRESS_LONG,                 // удержание дольше long_ms
    PRESS_DOUBLE,               // второй press в окне double_ms после release
    PRESS_SHORT_LONG,           // long после короткого press в окне double_ms
    PRESS_LONG_REPEAT,          // повтор long через repeat_ms пока держат
    PRESS_SHORT_LONG_REPEAT,    // повтор short_long через repeat_ms
} press_type_t;

typedef struct {
    void (*on_event)(unsigned num, press_type_t type);
    uint8_t * input_state;
    uint8_t * double_pressed;
    uint8_t * long_pressed;
    task_t  * task_long;
    task_t  * task_double;
    uint8_t  num;
    uint16_t long_ms;
    uint16_t double_ms;
    uint16_t repeat_ms;
} btn_press_processor_cfg_t;

void __bpp_long_to(unsigned ch);
void __bpp_dbl_to(unsigned ch);

#define BTN_PRESS_PROCESSOR_INIT_SIMPLE(_num, _on_event, _long_ms) { \
    .on_event       = _on_event, \
    .input_state    = (uint8_t [DIV_ROUND_UP(_num, 8)]){}, \
    .double_pressed = 0, \
    .long_pressed   = (uint8_t [DIV_ROUND_UP(_num, 8)]){}, \
    .task_long      = &(task_t) TASK_INIT_IDX(__bpp_long_to, _num), \
    .task_double    = 0, \
    .num            = _num, \
    .long_ms        = _long_ms, \
    .double_ms      = 0, \
    .repeat_ms      = 0, \
}

#define BTN_PRESS_PROCESSOR_INIT_FULL(_num, _on_event, _long_ms, _double_ms, _repeat_ms) { \
    .on_event       = _on_event, \
    .input_state    = (uint8_t [DIV_ROUND_UP(_num, 8)]){}, \
    .double_pressed = (uint8_t [DIV_ROUND_UP(_num, 8)]){}, \
    .long_pressed   = (uint8_t [DIV_ROUND_UP(_num, 8)]){}, \
    .task_long      = &(task_t) TASK_INIT_IDX(__bpp_long_to, _num), \
    .task_double    = &(task_t) TASK_INIT_IDX(__bpp_dbl_to,  _num), \
    .num            = _num, \
    .long_ms        = _long_ms, \
    .double_ms      = _double_ms, \
    .repeat_ms      = _repeat_ms, \
}

void init_btn_press_processor(const btn_press_processor_cfg_t * cfg);
void btn_press_processor_edge(unsigned ch, unsigned state);
