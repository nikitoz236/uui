#include <stdio.h>

#define DP_NOTABLE
#include "dp.h"

#include "text_pointer.h"



int main()
{
    dpn("test text pointer");

    tptr_t tp;
    text_ptr_init(&tp, 0, (xy_t){}, 0);

    return 0;
}
