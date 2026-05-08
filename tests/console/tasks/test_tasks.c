#include "str_utils.h"
#include "val_text.h"
#include "array_size.h"

#include "dp.h"
#include "tasks.h"
#include "eq.h"

static unsigned uptime_ms = 0;

unsigned get_uptime_ms(void)
{
    return uptime_ms;
}

static unsigned func_single_calls = 0;
static unsigned func_idx_calls[6] = {};

void func_single(void)
{
    func_single_calls++;
    dpn("        === func_single");
}

void func_idx(unsigned idx)
{
    func_idx_calls[idx]++;
    dp("        === func_idx"); dpd(idx, 2); dn();
}

static task_t task_single = TASK_INIT_SINGLE(func_single);
static task_t task_idx = TASK_INIT_IDX(func_idx, 6);

void eq_wrap(void)
{
    dpn("    eq process");
    while(eq_process()) {
        dpn("       ^ call");
    };
    dpn("    eq done");
}

void tasks_debug(void)
{
    dp("  task_single: "); dpx((unsigned)&task_single, 4); dp(" : "); dpxd(&task_single, 4, sizeof(task_single) / 4); dn();
    dp("  task_idx:    "); dpx((unsigned)&task_idx, 4); dp(" : "); dpxd(&task_idx, 4, sizeof(task_idx) / 4); dn();
}

int main()
{
    dpn("tasks test");
    unsigned total_res = 0;
    unsigned bad_cnt = 0;
    const char * res_str[] = { "OK", "FAIL" };

    dpn("");
    dpn("=== кейс 1: запуск двух тасков, ничего ещё не должно сработать ===");
    dpn("uptime=0, ставим task_idx[4] на 10мс (срабатывает в 10)");
    dpn("          ставим task_single на 20мс (срабатывает в 20)");
    tasks_debug();
    task_run_idx(&task_idx, 4, 10);
    task_run_single(&task_single, 20);
    tasks_debug();
    dpn("ожидаем: оба в списке, eq пустая, обработчики не вызывались");
    task_process();
    eq_wrap();

    dpn("");
    dpn("=== кейс 2: uptime=15 - должен сработать только task_idx[4] ===");
    uptime_ms = 15;
    dpn("ожидаем: func_idx 4 вызван один раз, task_single ещё в списке");
    task_process();
    eq_wrap();
    tasks_debug();

    dpn("");
    dpn("=== кейс 3: uptime=25 - должен сработать task_single ===");
    uptime_ms = 25;
    dpn("ожидаем: func_single вызван, ll-список пуст");
    task_process();
    eq_wrap();
    tasks_debug();

    dpn("");
    dpn("=== кейс 4: task_stop_idx до срабатывания ===");
    dpn("uptime=100, ставим task_idx[2] на 50мс (срабатывает в 150),");
    dpn("сразу останавливаем. uptime=200 - таймер давно бы вышел.");
    dpn("ожидаем: func_idx 2 НЕ вызван, task убран из ll-списка");
    unsigned before = func_idx_calls[2];
    uptime_ms = 100;
    task_run_idx(&task_idx, 2, 50);
    task_stop_idx(&task_idx, 2);
    uptime_ms = 200;
    task_process();
    eq_wrap();
    if (func_idx_calls[2] != before) {
        bad_cnt++;
        dpn("FAIL: остановленный таск сработал");
    } else {
        dpn("OK: остановленный таск не сработал");
    }

    dpn("");
    dpn("=== кейс 5: повторный task_run_idx перезапускает таймер ===");
    dpn("uptime=300, ставим task_idx[3] на 50мс (старый срок 350)");
    dpn("uptime=320, ставим task_idx[3] на 50мс (новый срок 370)");
    dpn("uptime=355, старый срок прошёл - но таймер был перезапущен");
    dpn("ожидаем: func_idx 3 ещё НЕ вызван");
    unsigned before5 = func_idx_calls[3];
    uptime_ms = 300;
    task_run_idx(&task_idx, 3, 50);
    uptime_ms = 320;
    task_run_idx(&task_idx, 3, 50);
    uptime_ms = 355;
    task_process();
    eq_wrap();
    if (func_idx_calls[3] != before5) {
        bad_cnt++;
        dpn("FAIL: таймер не перезапустился, сработал на старом сроке");
    } else {
        dpn("OK: на старом сроке не сработал");
    }
    dpn("uptime=375, новый срок прошёл");
    dpn("ожидаем: func_idx 3 вызван один раз");
    uptime_ms = 375;
    task_process();
    eq_wrap();
    if (func_idx_calls[3] != before5 + 1) {
        bad_cnt++;
        dp("FAIL: ожидался 1 вызов, получили "); dpd(func_idx_calls[3] - before5, 1); dn();
    } else {
        dpn("OK: сработал один раз на новом сроке");
    }

    dpn("");
    dpn("=== кейс 6: task_is_active_idx и task_remain_ms_idx ===");
    dpn("uptime=400, до запуска: is_active=0, remain=0");
    uptime_ms = 400;
    if (task_is_active_idx(&task_idx, 1) != 0) {
        bad_cnt++;
        dpn("FAIL: is_active до запуска не 0");
    }
    if (task_remain_ms_idx(&task_idx, 1) != 0) {
        bad_cnt++;
        dpn("FAIL: remain до запуска не 0");
    }
    dpn("ставим task_idx[1] на 100мс (срок 500)");
    task_run_idx(&task_idx, 1, 100);
    dp("сразу: is_active="); dpd(task_is_active_idx(&task_idx, 1), 1);
    dp(" remain="); dpd(task_remain_ms_idx(&task_idx, 1), 3); dn();
    dpn("ожидаем: is_active=1, remain=100");
    if (task_is_active_idx(&task_idx, 1) != 1) {
        bad_cnt++;
        dpn("FAIL: is_active после запуска не 1");
    }
    if (task_remain_ms_idx(&task_idx, 1) != 100) {
        bad_cnt++;
        dpn("FAIL: remain после запуска не 100");
    }
    uptime_ms = 430;
    dp("uptime=430: is_active="); dpd(task_is_active_idx(&task_idx, 1), 1);
    dp(" remain="); dpd(task_remain_ms_idx(&task_idx, 1), 3); dn();
    dpn("ожидаем: is_active=1, remain=70");
    if (task_remain_ms_idx(&task_idx, 1) != 70) {
        bad_cnt++;
        dpn("FAIL: remain не 70");
    }
    uptime_ms = 600;
    dp("uptime=600 (срок вышел): is_active="); dpd(task_is_active_idx(&task_idx, 1), 1);
    dp(" remain="); dpd(task_remain_ms_idx(&task_idx, 1), 3); dn();
    dpn("ожидаем: is_active=0, remain=0 (время уже вышло)");
    if (task_is_active_idx(&task_idx, 1) != 0) {
        bad_cnt++;
        dpn("FAIL: is_active после истечения не 0");
    }
    if (task_remain_ms_idx(&task_idx, 1) != 0) {
        bad_cnt++;
        dpn("FAIL: remain после истечения не 0");
    }
    task_process();
    eq_wrap();

    total_res = bad_cnt ? 1 : 0;
    dpn("");
    dp("total result: "); dp(res_str[total_res]); dp(" bad test "); dpd(bad_cnt, 1); dn();
    return total_res;
}
