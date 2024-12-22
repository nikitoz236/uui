#include <stdio.h>
#include <stdint.h>

#include "str_val.h"
#include "array_size.h"

int main()
{
    printf("test dec to str\r\n");

    struct {
        unsigned val;
        unsigned len;
        unsigned lzero;
    } test_data[] = {
        { .val = 0, .len = 6, .lzero = 0},
        { .val = 0, .len = 6, .lzero = 1},
        { .val = 0, .len = 1, .lzero = 0},
        { .val = 0, .len = 1, .lzero = 1},
        { .val = 1245678901, .len = 10, .lzero = 0},
        { .val = 1245678901, .len = 10, .lzero = 0},
    };

    const unsigned test_data_size = ARRAY_SIZE(test_data);

    for (unsigned i = 0; i < test_data_size; i++) {
        char str[16] = "xxxxxxxxxxxxxxxx";
        dec_to_str_right_aligned(test_data[i].val, str, test_data[i].len, test_data[i].lzero);
        printf("val: %14u, len: %2u, lzero: %1u, str: %16s\r\n", test_data[i].val, test_data[i].len, test_data[i].lzero, str);
    }

    return 0;
}
