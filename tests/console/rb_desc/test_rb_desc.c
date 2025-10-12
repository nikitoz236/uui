#include <stdio.h>

#include "rb.h"

#define DP_NOTABLE
#include "dp.h"

const rb_desc_t testrb = RB_DESC_INIT(uint32_t, 16);

int main()
{
    dp("test rb desc\n");


    dp("rb desc dump: "); dpxd(&testrb, 1, sizeof(rb_desc_t)); dn();
    // dp("rb ctx ptr: "); dpx(, 4); dn();



    return 0;
}
