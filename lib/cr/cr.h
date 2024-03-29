/*
корутины

идея

есть область памяти в которой висят флаги того что корутину можно продолжить
там же висит таймер - момент остановки и период когда надо возобновить

функции корутин не должны иметь стека, соответственно все циклы делаются на статических переменных
а вызовы функций также корутины ?

тоесть при каждом возобновлении весь стек должен заново развернуться до точки остановки

вызов функции сопровождается сохранением положения выполнения внутри функции, кроме того фунция возвращает всегда статус можно продолжать или выходим

причин для остановки может быть всего 2

это просто sleep либо ожидание чего то внешнего - не события скорее а завершение блокирующего состояния - завершение работы асинхронного процесса. а если мы ждем освобождение драйвера

могут ли быть несколько вызовов блокирующего состояния в одной корутине ?
ну например мы с одного интерфейса получили так еще и у другой сохраняем

события пусть запускают функции.



cr_stop(name, thread) - остановить корутину
cr_run(name, thread) - запускает корутину с нуля ??? или с места остановки ??? придумай сценарий
cr_unlock(name, thread) - разблокировать корутину из прерывания асинхронного действия
cr_error(name, thread, error) - асинхронное действие завершилось с ошибкой

cr_create(имя, количество экземпляров) {

} - создает корутину и возвращает указатель на контекст корутины

контекст содержит указатель на функицю и указатель на массив структур с контекстами
- таймер - ресет и корутина снова зайдет
- метрок где функция сейчас
- ждет ли нас другая корутина ? и с каким индексом
- состояний
    ждет асинхронного действия
    асинхронноге действие закончено
    статус асинхронного действия
    подготовиться к остановке
    остановлена

а если каждая вложенная функция есть корутина ? тогда можно вызывать из разных мест, но по завершению без ошибок нужно 


фишки
    - у корутин может быть несколько экземпляров, для обслуживания параллельных шин
    - корутина имеет контекст в котором видимо хранится метка перехода
    - таймер всегда крутится и используется для планирования запуска функции
    - корутина может быть останвлена или ждать события или ждать таймера

*/
#pragma once
#include <stdint.h>
// #include "mstimer.h"

typedef struct __cr_func cr_func_t;

typedef struct {
    // mstimer_t timer;
    const cr_func_t * current;
    const cr_func_t * who_call;
    uint8_t thread;
    uint8_t call_thread;
    uint16_t error;
} cr_ctx_t;

typedef enum {
    CR_SLEEP,           // ждем таймера
    CR_WAIT,            // ждем асинхронного действия которое гарантированно произойдет
    CR_WAIT_TIMEOUT,    // ждем асинхронного действия с таймаутом
    CR_STOP,            // остановлено
    CR_READY            // прямо сейчас надо запустить
} cr_state_t;

typedef struct {
    const struct __cr_func * call_process;
    void * pos;
    cr_state_t state;
    uint8_t call_thread;
} process_state_t;

struct __cr_func {
    void (*func)(cr_ctx_t * ctx);
    process_state_t * state;
    unsigned threads_num;
};

#define CR_ERROR_TIMEOUT                1
#define CR_ERROR_INVALID_THREAD         2

#define cr_create(name, __threads_num, code) \
    static void __cr_func_async_wrap_ ## name(cr_ctx_t * cr_ctx) \
    { \
        if (cr_ctx->current->state[cr_ctx->thread].pos != 0) { \
            goto *(cr_ctx->current->state[cr_ctx->thread].pos); \
        } \
        do { code } while (0); \
        cr_ctx->current->state[cr_ctx->thread].pos = 0; \
        cr_ctx->current->state[cr_ctx->thread].state = CR_STOP; \
        if (cr_ctx->who_call) { \
            /* то есть мы используем сохраненный номер вызвавшего нас потока */ \
            unsigned call_thread = cr_ctx->current->state[cr_ctx->thread].call_thread; \
            cr_ctx->who_call->state[call_thread].state = CR_READY; \
        } \
    }; \
    process_state_t __func_saved_ ## name[__threads_num]; \
    const cr_func_t name = { \
        .func = __cr_func_async_wrap_ ## name, \
        .state = __func_saved_ ## name, \
        .threads_num = __threads_num \
    };

static inline void ctx_print(cr_ctx_t * ctx)
{
    printf("    ctx %p", ctx);
    printf(" current %p state %p threads num %d\n", ctx->current, ctx->current->state, ctx->current->threads_num);
    for (unsigned i = 0; i < ctx->current->threads_num; i++) {
        printf("        thread %d state %d pos %p call_proc %p call_thread %d\n", i, ctx->current->state[i].state, ctx->current->state[i].pos, ctx->current->state[i].call_process, ctx->current->state[i].call_thread);
    }
}

static inline void func_print(cr_func_t * f)
{
    printf("    func %p threads %d state %p\n", f, f->threads_num, f->state);
    for (unsigned i = 0; i < f->threads_num; i++) {
        printf("        thread %d state %d pos %p call_proc %p call_thread %d\n", i, f->state[i].state, f->state[i].pos, f->state[i].call_process, f->state[i].call_thread);
    }
}

static inline void cr_current_init(cr_ctx_t * cr_ctx)
{
    unsigned thread = cr_ctx->thread;
    cr_ctx->current->state[thread].state = CR_READY;
    cr_ctx->current->state[thread].pos = 0;
    cr_ctx->current->state[thread].call_process = 0;
    cr_ctx->current->state[thread].call_thread = 0;
}

#define cr_lock() \
    /* cr_ctx и cr_thread доступны как локальные переменные функции, переданные ей в качестве аргументов */ \
    cr_ctx->current->state[cr_ctx->thread].state = CR_WAIT; \
    cr_ctx->current->state[cr_ctx->thread].pos = &&CR_LABEL; \
    return; \
    CR_LABEL:; \

static inline void cr_unlock(cr_ctx_t * ctx)
{
    ctx->current->state[ctx->thread].state = CR_READY;
}

#define cr_subprocess(sf, __new_thread) \
    /* cr_ctx и cr_thread доступны как локальные переменные функции, переданные ей в качестве аргументов */ \
    /* сохраняем текущее положение в функции */ \
    cr_ctx->current->state[cr_ctx->thread].pos = &&CR_LABEL; \
    cr_ctx->current->state[cr_ctx->thread].state = CR_WAIT; \
    /* вызывающая функция будет сохраняется для текущий потока выполнения */ \
    cr_ctx->current->state[cr_ctx->thread].call_process = cr_ctx->who_call; \
    cr_ctx->current->state[cr_ctx->thread].call_thread = cr_ctx->call_thread; \
    cr_ctx->who_call = cr_ctx->current; \
    cr_ctx->current = sf; \
    cr_ctx->call_thread = cr_ctx->thread; \
    cr_ctx->thread = __new_thread; \
    cr_current_init(cr_ctx); \
    cr_ctx->current->func(cr_ctx); \
    if (cr_ctx->who_call->state[cr_ctx->thread].state != CR_READY) { \
        return; \
    } \
    CR_LABEL:; \
    do { \
        cr_ctx->thread = cr_ctx->call_thread; \
        cr_ctx->current = cr_ctx->who_call; \
        cr_ctx->who_call = cr_ctx->current->state[cr_ctx->thread].call_process; \
        cr_ctx->call_thread = cr_ctx->current->state[cr_ctx->thread].call_thread; \
        cr_ctx->current->func(cr_ctx); \
    } while (0)

static inline void cr_resume_if_ready(cr_ctx_t * cr_ctx)
{
    if (cr_ctx->current->state[cr_ctx->thread].state == CR_READY) {
        cr_ctx->current->func(cr_ctx);
    }
}

static inline void cr_run(cr_ctx_t * cr_ctx, const cr_func_t * f, unsigned cr_thread)
{
    if (cr_thread > f->threads_num) {
        cr_ctx->error = CR_ERROR_INVALID_THREAD;
        return;
    }
    cr_ctx->who_call = 0;
    cr_ctx->current = f;
    cr_ctx->error = 0;
    cr_ctx->thread = cr_thread;
    cr_current_init(cr_ctx);

    // run!
    printf("run func %p\n", cr_ctx->current);
    cr_ctx->current->func(cr_ctx);
}

