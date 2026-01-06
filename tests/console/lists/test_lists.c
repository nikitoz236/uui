#include <stdio.h>

#define DP_NOTABLE
#include "dp.h"
#include "lists.h"

typedef struct {
    uint8_t x8;
    uint32_t x32;
    uint16_t x16;
} some_type_t;

list_t * some_type_list_ptr = LIST(((some_type_t []){
    { .x8 = 36, .x16 = 367, .x32 = 0xEEBACA11 },
    { .x8 = 46, .x16 = 467, .x32 = 0xEEBACA22 },
    { .x8 = 56, .x16 = 567, .x32 = 0xEEBACA33 },
}));

void pr(some_type_t * s)
{
    dp("some type struct process : x8 = "); dpd(s->x8, 3); dp(" x16 = "); dpd(s->x16, 5); dp(" x32 = "); dpx(s->x32, 4); dn();
}

int main()
{
    dpn("lists test");

    dpxd(some_type_list_ptr, 2, 2);
    dn();
    for (unsigned i = 0; i < some_type_list_ptr->count; i++) {
        void * ptr = &some_type_list_ptr->data[i * some_type_list_ptr->step];
        dpxd(ptr, 1, some_type_list_ptr->step); dn();
    }

    list_process_all(some_type_list_ptr, pr);


    return 0;
}
