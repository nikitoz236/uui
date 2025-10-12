#include <stdio.h>
#include "eq.h"

void test_noarg(void)
{
    printf("test_noarg\n");
}

void test_arg(unsigned arg)
{
    printf("test_arg: %d\n", arg);
}

int main()
{
    printf("eq test\n");

    eq_func_single(test_noarg);
    eq_func_idx(test_arg, 42);
    eq_func_idx(test_arg, 7);
    eq_func_single(test_noarg);

    while (eq_process()) {};

    return 0;
}
