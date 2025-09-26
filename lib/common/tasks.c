#include "tasks.h"
#include "mstimer.h"

struct task_ctx * task_list = 0;

/*
    у меня тут связанный список контекстов задач

    для добавления мне нужно 
*/

static struct task_ctx ** search_task_ctx(struct task_desc * task)
{
    // struct task_ctx * ctx = task_list;
    // while (ctx) {
    //     if (ctx->desc == task) {
}

void task_schedule_idx(struct task_ctx * task, unsigned idx, unsigned time_ms)
{
    // может выполнятся в любом контексте
    // может прерывать сама себя

    struct task_ctx * ctx = task_list;

    // указатель на переменную указатель куда записать новый элемент
    struct task_ctx ** list_prev = &task_list;

    while (ctx) {
        if (ctx == task) {
            break;
        }
        list_prev = &ctx->next;
        ctx = ctx->next;
    }

    if (ctx != task) {
        // добавляем последним элементом в список
        task->next = 0;
        *list_prev = task;
    }

    task->time_ms[idx] = uptime_ms + time_ms;
    task->active_masks |= (1 << idx);
}

void task_unschedule_idx(struct task_ctx * task, unsigned idx)
{
    // может выполнятся в любом контексте
    // может прерывать сама себя

    struct task_ctx * ctx = task_list;

    // указатель на переменную указатель куда записать новый элемент
    struct task_ctx ** list_prev = &task_list;

    while (ctx) {
        if (ctx == task) {
            break;
        }
        list_prev = &ctx->next;
        ctx = ctx->next;
    }

    if (ctx == task) {
        ctx->active_masks &= ~(1 << idx);
        if (ctx->active_masks == 0) {
            // нужно еще удалить из списка
            *list_prev = ctx->next;
            ctx->next = 0;
        }
    }
}

unsigned task_is_active_idx(struct task_ctx * task, unsigned idx)
{
    if (task->active_masks & (1 << idx)) {
        return 1;
    }
    return 0;
}

unsigned task_time_remain_idx(struct task_ctx * task, unsigned idx)
{
    if (task->active_masks & (1 << idx)) {
        return task->time_ms[idx] - uptime_ms;
    }
    return 0;
}

void process_tasks(void)
{
    // всегда выполняется в background

    struct task_ctx * ctx = task_list;
    struct task_ctx ** list_prev = &task_list;

    while (ctx) {
        // unsigned count = ctx->desc->count;
        // if (count > 32) {
        //     count = 32; // ограничение по количеству масок
        // }
        // uint32_t mask = 1;

        // for (unsigned i = 0; i < count; i++) {
        //     if (ctx->active_masks & mask) {
        //         if (__time_rel(uptime_ms, ctx->time_ms[i]) < 0) {
        //             ctx->active_masks &= ~mask; // снимаем активность до вызова функции, в функции может перезапуститься
        //             if (count == 1) {
        //                 ctx->desc->func();
        //             } else {
        //                 ctx->desc->func_idx(i);
        //             }
        //         }
        //     }
        // }

        // if (ctx->active_masks == 0) {
        //     // нужно еще удалить из списка
        //     *list_prev = ctx->next;
        // }

        list_prev = &ctx->next;
        ctx = ctx->next;
    }
}
