#include <stdio.h>

#include "str_utils.h"
#include "val_text.h"
#include "array_size.h"

#define DP_NOTABLE
#include "dp.h"
#include "tasks.h"
#include "eq.h"

volatile unsigned uptime_ms = 0;

void func_single(void)
{
    dpn("        === func_single");
}

void func_idx(unsigned idx)
{
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
    dp("tasks test\n");
    unsigned total_res = 0;
    unsigned bad_cnt = 0;
    const char * res_str[] = { "OK", "FAIL" };

    tasks_debug();
    task_run_idx(&task_idx, 4, 10);
    task_run_single(&task_single, 20);


    tasks_debug();
    task_process();
    eq_wrap();

    uptime_ms = 15;
    dpn("uptime 15");
    task_process();
    eq_wrap();
    tasks_debug();

    uptime_ms = 25;
    dpn("uptime 25");
    task_process();
    eq_wrap();
    tasks_debug();

    printf("total result: %s bad test %d\n", res_str[total_res], bad_cnt);
    return total_res;
}
