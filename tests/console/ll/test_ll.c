#include <stdio.h>

#define DP_NOTABLE
#include "dp.h"
#include "ll.h"
#include "flex_wrap.h"

struct payload {
    uint32_t val;
};

struct {
    ll_item_t item;
    struct payload pl;
} l[4] = {};

unsigned delete_val = 0;

unsigned process(struct payload * pl)
{
    dp("    process payload "); dpx(pl->val, 4);
    if (pl->val == delete_val) {
        dpn(" <- deleting ");
        return 1;
    }
    dn();
    return 0;
}

ll_item_t * list = 0;

void print_ctx(char * msg)
{
    dn(); 
    dpn(msg); 
    dp("  MEM: head ptr "); dpx((unsigned)list, 4); dn();
    for (unsigned i = 0; i < 4; i++) {
        dp("      "); dpx((unsigned)&l[i], 4); dp(" : "); dpxd(&l[i], 4, sizeof(l[0]) / 4); dn();
    }
}

int main()
{
    dp("ll test\n");
    unsigned total_res = 0;
    unsigned bad_cnt = 0;
    const char * res_str[] = { "OK", "FAIL" };

    for (unsigned i = 0; i < 4; i++) {
        l[i].pl.val = i + 0xAE00;
    }

    dp("l ptr: "); dpx((unsigned)l, 4); dn();

    print_ctx("init state");
    /*
        надо проверить что элементы связываются. как происходит обработка
        как они удаляются специально
        добавление если уже есть
        удаление если нет
        удаление в процессе обработки
    */

    ll_add(&list, &l[2].item);
    ll_add(&list, &l[0].item);
    ll_add(&list, &l[1].item);

    print_ctx("after adding 2,0,1: ");
    ll_process(&list, (unsigned(*)(void *))process);

    ll_remove(&list, &l[3].item); // нет такого
    print_ctx("after removing 3 (no such): ");
    ll_process(&list, (unsigned(*)(void *))process);


    ll_add(&list, &l[0].item);
    print_ctx("after adding 0 (already in list): ");
    ll_process(&list, (unsigned(*)(void *))process);

    dn(); dp("---- processing with delete 0xAE01 ----"); dn();
    delete_val = 0xAE00;
    ll_process(&list, (unsigned(*)(void *))process);
    print_ctx("after processing with delete 0: ");
    delete_val = 0;
    ll_process(&list, (unsigned(*)(void *))process);

    ll_add(&list, &l[3].item);
    print_ctx("after adding 3: ");
    ll_process(&list, (unsigned(*)(void *))process);

    ll_remove(&list, &l[1].item);
    print_ctx("after removing 1: ");
    ll_process(&list, (unsigned(*)(void *))process);

    ll_remove(&list, &l[2].item);
    print_ctx("after removing 2: ");
    ll_process(&list, (unsigned(*)(void *))process);

    printf("total result: %s bad test %d\n", res_str[total_res], bad_cnt);
    return total_res;
}
