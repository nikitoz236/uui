#include <stdio.h>

#include "tl.h"

#define DP_NOTABLE
#include "dp.h"

volatile unsigned uptime_ms = 0;

tl_t * testtl = TL_CREATE(16);


int main()
{
    dp("test tl desc\n");


    dp("tl desc dump: "); dpxd(testtl, 4, 1 + 16); dn();

    uptime_ms = 1000;
    tl_run(testtl, 0, 500);
    tl_run(testtl, 1, 1500);
    tl_run(testtl, 2, 2500);

    dp("after run 0 1 2: "); dpxd(testtl, 4, 1 + 16); dn();

    for (unsigned i = 0; i < 16; i++) {
        unsigned r = tl_remain_ms(testtl, i);
        dp("remain idx "); dpd(i, 2); dp(" ms "); dpd(r, 5); dn();
    }



    return 0;
}
