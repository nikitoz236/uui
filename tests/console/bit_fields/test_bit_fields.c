#include <stdio.h>
#include <string.h>
#include "array_size.h"

#include "bit_fields.h"


struct write_test {
    void * before;
    void * after;
    unsigned len;       // длина массива в байтах
    unsigned field_size;
    unsigned index;
    unsigned value;
};


struct read_test {
    void * data;
    unsigned len;       // длина массива в байтах
    unsigned field_size;
    unsigned index;
    unsigned expected_value;
};


struct write_test wt[] = {
    { .before = &(uint32_t[]){0xAABBCCDD}, .len = 4, .field_size = 1, .index = 1, .value = 1, .after = &(uint32_t[]){0xAABBCCDF} },
    { .before = &(uint32_t[]){0xAABBCCDD}, .len = 4, .field_size = 1, .index = 0, .value = 0, .after = &(uint32_t[]){0xAABBCCDC} },
    { .before = &(uint32_t[]){0}, .len = 4, .field_size = 1, .index = 22, .value = 1, .after = &(uint32_t[]){0x00400000} },

    { .before = &(uint32_t[]){0}, .len = 4, .field_size = 2, .index = 7, .value = 1, .after = &(uint32_t[]){0x00004000} },
    { .before = &(uint32_t[]){0}, .len = 4, .field_size = 2, .index = 7, .value = 2, .after = &(uint32_t[]){0x00008000} },
    { .before = &(uint32_t[]){0}, .len = 4, .field_size = 2, .index = 7, .value = 3, .after = &(uint32_t[]){0x0000C000} },

    { .before = &(uint32_t[]){0xAABBCCDD}, .len = 4, .field_size = 4, .index = 2, .value = 0x2, .after = &(uint32_t[]){0xAABBC2DD} },
    { .before = (uint8_t[]){0xAA, 0xAA, 0xAA, 0xAA}, .len = 4, .field_size = 4, .index = 6, .value = 0xC, .after = (uint8_t[]){0xAA, 0xAA, 0xAA, 0xAC} },
    { .before = (uint16_t[]){0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD}, .len = 8, .field_size = 4, .index = 7, .value = 0xE, .after = (uint16_t[]){0xAAAA, 0xEBBB, 0xCCCC, 0xDDDD} },
    { .before = (uint16_t[]){0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD}, .len = 8, .field_size = 1, .index = 34, .value = 0x0, .after = (uint16_t[]){0xAAAA, 0xBBBB, 0xCCC8, 0xDDDD} }
};

struct read_test rt[] = {
    { .data = &(uint32_t[]){0xAABBCCDD}, .len = 4, .field_size = 1, .index = 0, .expected_value = 1 },
    { .data = &(uint32_t[]){0xAABBCCDD}, .len = 4, .field_size = 1, .index = 1, .expected_value = 0 },
    { .data = &(uint32_t[]){0xAABBCCDD}, .len = 4, .field_size = 1, .index = 2, .expected_value = 1 },
    { .data = &(uint32_t[]){0xAABBCCDD}, .len = 4, .field_size = 1, .index = 3, .expected_value = 1 },

    { .data = &(uint32_t[]){0xAABBCCDD}, .len = 4, .field_size = 2, .index = 0, .expected_value = 1 },
    { .data = &(uint32_t[]){0xAABBCCDD}, .len = 4, .field_size = 2, .index = 1, .expected_value = 3 },
    { .data = &(uint32_t[]){0xAABBCCDD}, .len = 4, .field_size = 2, .index = 2, .expected_value = 0 },

};

void print_hex(void * array, unsigned len)
{
    uint8_t * p = (uint8_t *)array;
    for (unsigned i = 0; i < len; i++) {
        printf("%02X ", p[i]);
    }
}

unsigned do_wr_test(unsigned tn)
{
    struct write_test * t = &wt[tn];

    printf("Test %2u: [len: %d, fs: %d, idx: %d, val %X]  ", tn, t->len, t->field_size, t->index, t->value);
    print_hex(t->before, t->len);
    printf("-> ");

    bf_set(t->before, t->index, t->field_size, t->value);

    print_hex(t->before, t->len);
    printf(" wait: ");
    print_hex(t->after, t->len);

    if (memcmp(t->before, t->after, t->len) != 0) {
        printf("   FAILED\n");
        return 1;
    } else {
        printf("   PASSED\n");
        return 0;
    }
}

unsigned do_rd_test(unsigned tn)
{
    struct read_test * t = &rt[tn];

    printf("Test %2u: [len: %d, fs: %d, idx: %d]  ", tn, t->len, t->field_size, t->index);
    print_hex(t->data, t->len);
    printf("-> ");

    unsigned v = bf_get(t->data, t->index, t->field_size);

    printf("val %X wait %X", v, t->expected_value);

    if (v != t->expected_value) {
        printf("   FAILED\n");
        return 1;
    } else {
        printf("   PASSED\n");
        return 0;
    }
}

int main()
{
    printf("bit fields write test\n");

    unsigned res = 0;
    printf("Write tests:\n");
    for (unsigned i = 0; i < ARRAY_SIZE(wt); i++) {
        if (do_wr_test(i)) {
            res = 1;
        }
    }

    printf("Read tests:\n");
    for (unsigned i = 0; i < ARRAY_SIZE(rt); i++) {
        if (do_rd_test(i)) {
            res = 1;
        }
    }

    if (res == 0) {
        printf("PASSED\n");
        return 0;
    } else {
        printf("FAILED\n");
        return 1;
    }
}
