#include <stdio.h>
#include <stdint.h>

#include "str_val.h"

void print_hex(uint8_t *data, int len)
{
    for (int i = 0; i < len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\r\n");
}

int main()
{
    printf("test hex dump\r\n");

    uint8_t data[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

    char dump[64] = {};

    for (int i = 0; i < 16; i++) {
        dump[i] = 0xFF;
    }
    hex_to_str(&data[2], dump, 1);

    print_hex(dump, 16);
    print_hex(data, 16);

    printf("hex 8: %s\r\n", dump);

    hex_dump_to_str(data, dump, 1, 16);
    printf("dump 8: %s\r\n", dump);

    // test 16 bit hex print

    hex_to_str(&data[2], dump, 2);
    printf("hex 16: %s\r\n", dump);

    hex_dump_to_str(data, dump, 2, 8);
    printf("dump 16: %s\r\n", dump);

    // test 32 bit hex print

    hex_to_str(&data[2], dump, 4);
    printf("hex 32: %s\r\n", dump);

    hex_dump_to_str(data, dump, 4, 4);
    printf("dump 32: %s\r\n", dump);

    return 0;
}
