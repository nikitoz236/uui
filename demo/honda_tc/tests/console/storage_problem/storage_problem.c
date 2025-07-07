#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "storage.h"
#include "storage_hw.h"

#include "emu_storage.h"

int main ()
{
    printf("storage lib test\n");

    emu_storage_load();
    storage_init();

    printf("\n\n\n\n\nREADY\n");
    storage_print_info();

    // print_hex_dump(storage_page_to_pointer(0), 32);
    return 0;
}
