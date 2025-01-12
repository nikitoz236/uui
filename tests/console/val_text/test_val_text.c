#include <stdio.h>

#include "str_utils.h"
#include "val_text.h"
#include "array_size.h"

int8_t tv_s8_p = 45;
int8_t tv_s8_m = -12;
uint8_t tv_u8_1 = 3;
uint8_t tv_u8_2 = 243;

int16_t tv_s16_p = 31334;
int16_t tv_s16_m = -5314;
uint16_t tv_u16_1 = 34;
uint16_t tv_u16_2 = 54321;
int32_t tv_s32_p = 1234536;
int32_t tv_s32_m = -12356;
uint32_t tv_u32_1 = 3456;
uint32_t tv_u32_2 = 4294967290;
uint32_t tv_u32_p = 9765400;

struct {
    val_text_t tv;
    void * ptr;
    char * es;
    unsigned el;
} tests[] = {
    //                                                                                                                         1234567890123456789
    { .tv = { .t = DEC, .vs = VAL_SIZE_8,  .s = 1, .p = 0, .l =  4, .zl = 0, .zr = 0, .f = X1}, .ptr = &tv_s8_p,        .es = "  45"                },      //    1
    { .tv = { .t = DEC, .vs = VAL_SIZE_8,  .s = 1, .p = 0, .l =  4, .zl = 0, .zr = 0, .f = X1}, .ptr = &tv_s8_m,        .es = "- 12"                },      //    2
    { .tv = { .t = DEC, .vs = VAL_SIZE_8,  .s = 0, .p = 0, .l =  4, .zl = 0, .zr = 0, .f = X1}, .ptr = &tv_u8_1,        .es = "   3"                },      //    3
    { .tv = { .t = DEC, .vs = VAL_SIZE_8,  .s = 0, .p = 0, .l =  4, .zl = 0, .zr = 0, .f = X1}, .ptr = &tv_u8_2,        .es = " 243"                },      //    4

    { .tv = { .t = DEC, .vs = VAL_SIZE_8,  .s = 0, .p = 0, .l = 12, .zl = 0, .zr = 0, .f = X1}, .ptr = &tv_u8_2,        .es = "         243"        },      //    5

    { .tv = { .t = DEC, .vs = VAL_SIZE_16, .s = 1, .p = 0, .l =  6, .zl = 0, .zr = 0, .f = X1}, .ptr = &tv_s16_p,       .es = " 31334"              },      //    6
    { .tv = { .t = DEC, .vs = VAL_SIZE_16, .s = 1, .p = 0, .l =  6, .zl = 0, .zr = 0, .f = X1}, .ptr = &tv_s16_m,       .es = "- 5314"              },      //    7
    { .tv = { .t = DEC, .vs = VAL_SIZE_16, .s = 0, .p = 0, .l =  6, .zl = 0, .zr = 0, .f = X1}, .ptr = &tv_u16_1,       .es = "    34"              },      //    8
    { .tv = { .t = DEC, .vs = VAL_SIZE_16, .s = 0, .p = 0, .l =  6, .zl = 0, .zr = 0, .f = X1}, .ptr = &tv_u16_2,       .es = " 54321"              },      //    9

    { .tv = { .t = DEC, .vs = VAL_SIZE_32, .s = 1, .p = 0, .l = 10, .zl = 0, .zr = 0, .f = X1}, .ptr = &tv_s32_p,       .es = "   1234536"          },      //   10
    { .tv = { .t = DEC, .vs = VAL_SIZE_32, .s = 1, .p = 0, .l = 10, .zl = 0, .zr = 0, .f = X1}, .ptr = &tv_s32_m,       .es = "-    12356"          },      //   11
    { .tv = { .t = DEC, .vs = VAL_SIZE_32, .s = 0, .p = 0, .l = 10, .zl = 0, .zr = 0, .f = X1}, .ptr = &tv_u32_1,       .es = "      3456"          },      //   12
    { .tv = { .t = DEC, .vs = VAL_SIZE_32, .s = 0, .p = 0, .l = 10, .zl = 0, .zr = 0, .f = X1}, .ptr = &tv_u32_2,       .es = "4294967290"          },      //   13

    { .tv = { .t = DEC, .vs = VAL_SIZE_32, .s = 1, .p = 0, .l = 10, .zl = 1, .zr = 0, .f = X1}, .ptr = &tv_s32_p,       .es = "0001234536"          },      //   14
    { .tv = { .t = DEC, .vs = VAL_SIZE_32, .s = 1, .p = 0, .l = 10, .zl = 1, .zr = 0, .f = X1}, .ptr = &tv_s32_m,       .es = "-000012356"          },      //   15
    { .tv = { .t = DEC, .vs = VAL_SIZE_32, .s = 0, .p = 0, .l = 10, .zl = 1, .zr = 0, .f = X1}, .ptr = &tv_u32_1,       .es = "0000003456"          },      //   16
    { .tv = { .t = DEC, .vs = VAL_SIZE_32, .s = 0, .p = 0, .l = 10, .zl = 1, .zr = 0, .f = X1}, .ptr = &tv_u32_2,       .es = "4294967290"          },      //   17

    { .tv = { .t = DEC, .vs = VAL_SIZE_8,  .s = 0, .p = 0, .l = 12, .zl = 1, .zr = 0, .f = X1}, .ptr = &tv_u8_2,        .es = "000000000243"        },      //   18

    { .tv = { .t = DEC, .vs = VAL_SIZE_32, .s = 0, .p = 0, .l =  7, .zl = 0, .zr = 0, .f = X1}, .ptr = &tv_u32_2,       .es = "@967290"             },      //   19
    { .tv = { .t = DEC, .vs = VAL_SIZE_32, .s = 0, .p = 3, .l = 10, .zl = 0, .zr = 0, .f = X1000},  .ptr = &tv_s32_p,   .es = "  1234.536"          },      //   20
    { .tv = { .t = DEC, .vs = VAL_SIZE_32, .s = 0, .p = 1, .l = 10, .zl = 0, .zr = 0, .f = X1000},  .ptr = &tv_s32_p,   .es = "    1234.5"          },      //   21
    { .tv = { .t = DEC, .vs = VAL_SIZE_32, .s = 0, .p = 3, .l = 10, .zl = 0, .zr = 0, .f = X10},    .ptr = &tv_s32_p,   .es = "123453.6  "          },      //   22
    { .tv = { .t = DEC, .vs = VAL_SIZE_32, .s = 0, .p = 3, .l = 10, .zl = 0, .zr = 1, .f = X10},    .ptr = &tv_s32_p,   .es = "123453.600"          },      //   23
    { .tv = { .t = DEC, .vs = VAL_SIZE_32, .s = 0, .p = 3, .l = 10, .zl = 0, .zr = 0, .f = X1000},  .ptr = &tv_u32_p,   .es = "  9765.4  "          },      //   24
    { .tv = { .t = DEC, .vs = VAL_SIZE_32, .s = 0, .p = 2, .l = 10, .zl = 0, .zr = 0, .f = X1000},  .ptr = &tv_u32_p,   .es = "   9765.4 "          },      //   25
};



int main()
{
    printf("val text convert test\n");
    unsigned total_res = 0;
    unsigned bad_cnt = 0;
    const char * res_str[] = { "OK", "FAIL" };
    for (unsigned i = 0; i < ARRAY_SIZE(tests); i++) {
        char str[20];
        unsigned res = 0;
        unsigned exp_len = str_len(tests[i]     .es, 20);

        val_text_to_str(str, tests[i].ptr, &tests[i].tv);

        unsigned res_len = str_len(str, 20);
        if (res_len != exp_len) {
            res = 1;
        }
        if (str_cmp(str, tests[i]       .es, exp_len) == 0) {
            res = 1;
        }

        printf("  TEST: %4d         [%2d] %20s      expect: [%2d] %20s     %s\r\n", i + 1, res_len, str, exp_len, tests[i].es, res_str[res]);

        if (res) {
            total_res = 1;
            bad_cnt++;
        }
    }
    printf("total result: %s bad test %d\n", res_str[total_res], bad_cnt);
    return total_res;
}
