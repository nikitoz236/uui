#include <stdio.h>

#include "rb.h"

#define DP_NOTABLE
#include "dp.h"

RB_CREATE(testrb, 16, 4);

int main()
{
    dp("test rb desc\n");


    dp("rb desc dump: "); dpxd(&testrb, 1, sizeof(rb_desc_t)); dn();
    dp("rb ctx ptr: "); dpx(&rb_testrb_ctx, 4); dn();



    return 0;
}
