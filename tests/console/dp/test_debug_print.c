#include <stdio.h>
#include "dp.h"

int main()
{
    printf("debug print test\n");

    dp("fuck! this val: "); dpd(456, 5); dpn(" so ...");
    dpn("just next str");
    dp("hex val: "); dpx(0xABCD, 2); dn();
    dp("hex dump: "); dpxd(&main, 1, 16); dn();

    dpl("fuck", 7); dpn("<");

    return 0;
}
