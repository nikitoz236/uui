#include "tasks.h"
#include "eq.h"

#define DP_NAME "tasks"
#include "dp.h"

static ll_item_t * task_list = 0;

static unsigned has_active(const task_desc_t * desc, unsigned now)
{
    for (unsigned i = 0; i < desc->count; i++) {
        if (t32_is_active(&desc->timers[i], now)) {
            return 1;
        }
    }
    return 0;
}

void task_run_idx(task_t * task, unsigned idx, unsigned time_ms)
{
    if (idx >= task->desc->count) {
        return;
    }
    dp("task_run_idx: "); dpx((unsigned)task, 4); dn();
    t32_run(&task->desc->timers[idx], get_uptime_ms(), time_ms);
    ll_add(&task_list, (ll_item_t *)task);
}

void task_stop_idx(task_t * task, unsigned idx)
{
    if (idx >= task->desc->count) {
        return;
    }
    unsigned now = get_uptime_ms();
    if (t32_is_active(&task->desc->timers[idx], now)) {
        t32_stop(&task->desc->timers[idx]);
        if (!has_active(task->desc, now)) {
            ll_remove(&task_list, &task->ll_item);
        }
    }
}

static unsigned ll_task_process(task_desc_t ** desc_ptr)
{
    task_desc_t * desc = *desc_ptr;
    unsigned now = get_uptime_ms();

    for (unsigned i = 0; i < desc->count; i++) {
        if (t32_is_over(&desc->timers[i], now)) {
            t32_stop(&desc->timers[i]);
            if (desc->type == TASK_TYPE_SINGLE) {
                eq_func_single(desc->func_noarg);
            } else {
                eq_func_idx(desc->func_idx, i);
            }
        }
    }
    if (!has_active(desc, now)) {
        return 1; // удалить из списка
    }
    return 0; // оставить в списке
}

void task_process(void)
{
    dp("task_process. head : "); dpx((unsigned)task_list, 4); dn();
    ll_process(&task_list, (unsigned(*)(void *))ll_task_process);
}




/*
опасность то была что мы можем выполнять в контексте прерывания добавление

АГА
не время планировать.

в прерываниях мы скорее всего просто будем обработчик кидать в очередь




конкретно сейчас я хочу 

ооооо
у меня будет i2c на корутинах
и будет дребезг
дребезг делаем через просто например 8 клавиш. типа есть таблица с номером клавиши когда она была нажата

если там нет то добавляем и ставим таймер на время дребезга
по срезу 

дальше 

*/
