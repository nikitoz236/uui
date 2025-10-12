#include "tasks.h"
#include "mstimer.h"

struct task_ctx * task_list = 0;

/*
    у меня тут связанный список контекстов задач

    для добавления мне нужно 

    надо проверять индексы
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
    // может прерывать сама себя - нужно с атомарностю разобраться

    if (idx >= 32) {
        return;
    }

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
    // может прерывать сама себя - нужно с атомарностю разобраться

    if (idx >= 32) {
        return;
    }

    struct task_ctx * ctx = task_list;

    // указатель на переменную указатель которая ссылалась на удаляемый элемент
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
    if (idx >= 32) {
        return 0;
    }
    if (task == 0) {
        return 0;
    }
    if (task->active_masks & (1 << idx)) {
        return 1;
    }
    return 0;
}

unsigned task_time_remain_idx(struct task_ctx * task, unsigned idx)
{
    if (idx >= 32) {
        return 0;
    }
    if (task == 0) {
        return 0;
    }
    if (task->active_masks & (1 << idx)) {
        return task->time_ms[idx] - uptime_ms;
    }
    return 0;
}

void process_tasks(void)
{
    // всегда выполняется в background

    struct task_ctx * ctx = task_list;

    // указатель на переменную указатель которая ссылалась на удаляемый элемент
    struct task_ctx ** list_prev = &task_list;

    while (ctx) {
        unsigned count = ctx->desc->count;
        printf("process task %p count %u\r\n", ctx, count);
        if (count > 32) {
            count = 32; // ограничение по количеству масок
        }

        uint32_t active_bkp = ctx->active_masks;
        uint32_t mask = 1;
        struct task_ctx * next_ctx = ctx->next; // сохраняем следующий элемент заранее

        for (unsigned i = 0; i < count; i++) {
            if (ctx->active_masks & mask) {
                printf(" check idx %u time %u uptime %u\r\n", i, ctx->time_ms[i], uptime_ms);
                if (__time_rel(uptime_ms, ctx->time_ms[i]) < 0) {
                    printf("  task %p idx %u timeout\r\n", ctx, i);
                    ctx->active_masks &= ~mask; // снимаем активность до вызова функции, в функции может перезапуститься
                    // функция может дернуть schedule/unschedule и поменять дерево
                    // допустим что элемент еще в списке, но у него все нет активных индексов
                    // значит нам надо такое проверять гдето
                    // либо прямо здесь удалить элемент из списка

                    if (ctx->active_masks == 0) {
                        // нужно еще удалить из списка
                        *list_prev = ctx->next;
                        ctx->next = 0;
                    }

                    if (count == 1) {
                        ctx->desc->func_single();
                    } else {
                        ctx->desc->func_idx(i);
                    }
                }
            }
            mask <<= 1;
        }

        if (ctx->active_masks) {
            list_prev = &ctx->next;
        }

        ctx = next_ctx;
    }
}


/*
    пиздец конечно идея
    типа не надо таскменеджер с проверкой времени


    а если сделать


    задачи какие бывают

    засечь время
    проверить активно ли время
    выполнить чтото когда время выйдет

    поэтому нам нужна очередь выполнения !!!
    типа колбэк + аргумент которые залетают в очередь. а выполняются потом
    переодические штуки может обременять периодом ?


    типа у тебя есть сущности
        просто временная метка микросекундная
        временная метка точная

        временная метка + период






*/

typedef uint32_t timer_ms_t;

struct scheduler_item {
    struct exec_item * next;
    uint16_t idx;
    uint16_t has_arg : 1;
};

struct task {
    struct scheduler_item * item;
    timer_ms_t time_ms;
    uint32_t period;
    // если 0 то не периодическая
    // если в очереди то активная
};
