#pragma once
#include <stdint.h>

/*
    антидребезг сигналов с N входов

    на каждый сырой edge (пере)запускается таймер debounce_ms.
    когда таймер истёк без новых edge - стабильное состояние
    зафиксировано, генерируется on_edge(num, state).

    память: 2 битмапа raw_state и stable_state по DIV_ROUND_UP(num,8) байт
            + task с timer_32_t[num].

    инициализация - макрос разворачивается в { ... } инициализатор:
        static const btn_debounce_cfg_t my =
            BTN_DEBOUNCE_INIT(5, my_handler, 50);
*/

#include "tasks.h"
#include "round_up.h"

typedef struct {
    void (*on_edge)(unsigned num, unsigned state);
    uint8_t * raw_state;
    uint8_t * stable_state;
    task_t  * task;
    uint8_t  num;
    uint16_t debounce_ms;
} btn_debounce_cfg_t;

void __bdb_to(unsigned ch);

#define BTN_DEBOUNCE_INIT(_num, _on_edge, _debounce_ms) { \
    .on_edge      = _on_edge, \
    .raw_state    = (uint8_t [DIV_ROUND_UP(_num, 8)]){}, \
    .stable_state = (uint8_t [DIV_ROUND_UP(_num, 8)]){}, \
    .task         = &(task_t) TASK_INIT_IDX(__bdb_to, _num), \
    .num          = _num, \
    .debounce_ms  = _debounce_ms, \
}

void init_btn_debounce(const btn_debounce_cfg_t * cfg);
void btn_debounce_edge(unsigned ch, unsigned state);
