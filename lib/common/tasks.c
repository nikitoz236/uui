#include "tasks.h"
#include "eq.h"

#define DP_NAME "tasks"
#include "dp.h"

static ll_item_t * task_list = 0;

void task_run_idx(task_t * task, unsigned idx, unsigned time_ms)
{
    if (idx >= task->desc->count) {
        return;
    }
    dp("task_run_idx: "); dpx((unsigned)task, 4); dn();
    tl_run(task->desc->timers, idx, time_ms);
    ll_add(&task_list, (ll_item_t *)task);
}

void task_stop_idx(task_t * task, unsigned idx)
{
    if (idx >= task->desc->count) {
        return;
    }
    if (tl_is_active(task->desc->timers, idx)) {
        tl_stop(task->desc->timers, idx);
        if (task->desc->timers->active == 0) {
            ll_remove(&task_list, &task->ll_item);
        }
    }
}

static unsigned ll_task_process(task_desc_t ** desc_ptr)
{
    task_desc_t * desc = *desc_ptr;

    for (unsigned i = 0; i < desc->count; i++) {
        if (tl_is_over(desc->timers, i)) {
            tl_stop(desc->timers, i);
            if (desc->count == 0) {
                eq_func_single(desc->func_noarg);
            } else {
                eq_func_idx(desc->func_idx, i);
            }
        }
    }
    if (desc->timers->active == 0) {
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
