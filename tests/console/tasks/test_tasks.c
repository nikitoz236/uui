#include <stdio.h>

#include "str_utils.h"
#include "val_text.h"
#include "array_size.h"

#define DP_NOTABLE
#include "dp.h"
#include "tasks.h"

unsigned uptime_ms = 0xFABBCCDD;

void __debug_usart_tx_data(const char * s, unsigned len)
{
    while (len--) {
        putchar(*s++);
    }
}

void func_single(void)
{
    printf("func_single\n");
}

void func_idx(unsigned idx)
{
    printf("func_idx %d\n", idx);
}

TASK_SINGLE(func_single);
TASK_IDX(func_idx, 3);

void print_task_info(const void * ctx, const void * desc, const void * func, const void * array)
{
    dp("  func ptr : "); dpx((unsigned)func, 4); dn();
    dp("  desc : "); dpxd(desc, 4, sizeof(struct task_desc) / 4); dn();
    dp("  desc ptr : "); dpx((unsigned)desc, 4); dn();
    dp("  time array : "); dpx((unsigned)array, 4); dn();
    dp("  ctx ctx : "); dpxd(ctx, 4, sizeof(struct task_ctx) / 4); dn();
    dp("  ctx ptr : "); dpx((unsigned)ctx, 4); dn();
}

int main()
{
    dp("tasks test\n");
    unsigned total_res = 0;
    unsigned bad_cnt = 0;
    const char * res_str[] = { "OK", "FAIL" };

    dp("function func_single"); dn();
    print_task_info(&task_ctx_func_single, &__task_desc_func_single, func_single, __task_time_func_single); dn();

    dp("function func_idx"); dn();
    print_task_info(&task_ctx_func_idx, &__task_desc_func_idx, func_idx, __task_time_func_idx); dn();

    extern struct task_ctx * task_list;

    dp("task_list ptr : "); dpx((unsigned)task_list, 4); dn();

    task_schedule_idx(&task_ctx_func_idx, 2, 1000);

    dp("task_list ptr : "); dpx((unsigned)task_list, 4); dn();

    task_schedule_single(&task_ctx_func_single, 0x12345678);

    dp("task_list ptr : "); dpx((unsigned)task_list, 4); dn();

    dp("schedule func_single ctx : "); dpxd(&task_ctx_func_single, 4, sizeof(struct task_ctx) / 4);
    dp(" times: "); dpxd(__task_time_func_single, 4, sizeof(__task_time_func_single) / 4); dn();

    dp("schedule func_idx ctx : "); dpxd(&task_ctx_func_idx, 4, sizeof(struct task_ctx) / 4);
    dp(" times: "); dpxd(__task_time_func_idx, 4, sizeof(__task_time_func_idx) / 4); dn();

    task_unschedule_idx(&task_ctx_func_idx, 2);

    dp("task_list ptr : "); dpx((unsigned)task_list, 4); dn();

    dp("unschedule func_single ctx : "); dpxd(&task_ctx_func_single, 4, sizeof(struct task_ctx) / 4);
    dp(" times: "); dpxd(__task_time_func_single, 4, sizeof(__task_time_func_single) / 4); dn();

    dp("un  schedule func_idx ctx : "); dpxd(&task_ctx_func_idx, 4, sizeof(struct task_ctx) / 4);
    dp(" times: "); dpxd(__task_time_func_idx, 4, sizeof(__task_time_func_idx) / 4); dn();




    printf("total result: %s bad test %d\n", res_str[total_res], bad_cnt);
    return total_res;
}
