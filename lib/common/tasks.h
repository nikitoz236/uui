#pragma once
#include <stdint.h>

/*
    модуль для планирования задач, реализует связаный список контекстов задач

    где надо?
        антидребезг нескольких входов

    планирование таймаутов


    такуюже залупу надо на ожидание ресурса

    какие вообще есть варианты
        функция ждет таймаута, который нужно просто проверить
        функция отложена на какоето время
        функция обработчик таймаута + ожидание события
        просто запланировать функцию в бэкграунд из прерывания

*/

struct task_desc {
    union {
        void (*func_single)(void);                      // count == 1
        void (*func_idx)(unsigned idx);          // count != 1
    };
    uint8_t count;
};

struct task_ctx {
    struct task_ctx * next;
    const struct task_desc * desc;
    uint32_t active_masks;
    uint32_t * time_ms;
};

/*

тут 2 варианта
либо я делаю массив нулевой длины в структуре и ебусь с типом созданного контекста
либо храню указатеь на массив времен - РЕШИЛ ТАК
либо просто обьявляю массив после и надеюсь что линкер положит их рядом. хз можно ли на это надеяться

*/

#define TASK_IDX(f, c) \
    static const struct task_desc __task_desc_ ## f = { \
        .func_idx = f, \
        .count = c, \
    }; \
    uint32_t __task_time_ ## f[c] = {}; \
    static struct task_ctx task_ctx_ ## f = { \
        .next = 0, \
        .desc = & __task_desc_ ## f, \
        .time_ms = __task_time_ ## f, \
        .active_masks = 0, \
    };

#define TASK_SINGLE(f) \
    static const struct task_desc __task_desc_ ## f = { \
        .func_single = f, \
        .count = 1, \
    }; \
    uint32_t __task_time_ ## f[1] = {}; \
    static struct task_ctx task_ctx_ ## f = { \
        .next = 0, \
        .desc = & __task_desc_ ## f, \
        .time_ms = __task_time_ ## f, \
        .active_masks = 0, \
    };

void task_schedule_idx(struct task_ctx * task, unsigned idx, unsigned time_ms);
void task_unschedule_idx(struct task_ctx * task, unsigned idx);
unsigned task_is_active_idx(struct task_ctx * task, unsigned idx);
unsigned task_time_remain_idx(struct task_ctx * task, unsigned idx);

static inline void task_schedule_single(struct task_ctx * task, unsigned time_ms) { task_schedule_idx(task, 0, time_ms); };
static inline void task_unschedule_single(struct task_ctx * task) { task_unschedule_idx(task, 0); };
static inline unsigned task_is_active_single(struct task_ctx * task) { return task_is_active_idx(task, 0); };
static inline unsigned task_time_remain_single(struct task_ctx * task) { return task_time_remain_idx(task, 0); };

void process_tasks(void);
