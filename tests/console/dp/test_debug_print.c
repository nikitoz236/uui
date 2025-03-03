#include <stdio.h>

#define DP_NAME "main"
#include "dp.h"

void second_module_dp(void);

int main()
{
    printf("debug print test\n");

    dp("fuck! this val: "); dpd(456, 5); dpn(" so ...");
    dpn("just next str");
    dp("hex val: "); dpx(0xABCD, 2); dn();
    dp("hex dump: "); dpxd(&main, 1, 16); dn();

    dpl(0, 20); dpn("<");

    second_module_dp();

    dpl("fuck", 7); dpn("<");

    dpct(DPC_RED); dpn("color text red");
    dpn("next str without color");
    dpct(DPC_RED); dp("color text red "); dpcr(); dpn("reset color in midle of str");

    return 0;
}
