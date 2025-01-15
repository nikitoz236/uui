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
uint32_t tv_u32_0 = 0;

struct {
    val_text_t tv;
    val_rep_t rep;
    void * ptr;
    char * es;
    unsigned l;
    unsigned el;
} tests[] = {
    //                                                                                                                         1234567890123456789
    { .rep = { .vs = VAL_SIZE_8,  .s = 1 }, .tv = { .p = 0, .zl = 0, .zr = 0, .f = X1},    .l =  4, .ptr = &tv_s8_p,    .es = "  45"                },      //    1
    { .rep = { .vs = VAL_SIZE_8,  .s = 1 }, .tv = { .p = 0, .zl = 0, .zr = 0, .f = X1},    .l =  4, .ptr = &tv_s8_m,    .es = "- 12"                },      //    2
    { .rep = { .vs = VAL_SIZE_8,  .s = 0 }, .tv = { .p = 0, .zl = 0, .zr = 0, .f = X1},    .l =  4, .ptr = &tv_u8_1,    .es = "   3"                },      //    3
    { .rep = { .vs = VAL_SIZE_8,  .s = 0 }, .tv = { .p = 0, .zl = 0, .zr = 0, .f = X1},    .l =  4, .ptr = &tv_u8_2,    .es = " 243"                },      //    4

    { .rep = { .vs = VAL_SIZE_8,  .s = 0 }, .tv = { .p = 0, .zl = 0, .zr = 0, .f = X1},    .l = 12, .ptr = &tv_u8_2,    .es = "         243"        },      //    5

    { .rep = { .vs = VAL_SIZE_16, .s = 1 }, .tv = { .p = 0, .zl = 0, .zr = 0, .f = X1},    .l =  6, .ptr = &tv_s16_p,   .es = " 31334"              },      //    6
    { .rep = { .vs = VAL_SIZE_16, .s = 1 }, .tv = { .p = 0, .zl = 0, .zr = 0, .f = X1},    .l =  6, .ptr = &tv_s16_m,   .es = "- 5314"              },      //    7
    { .rep = { .vs = VAL_SIZE_16, .s = 0 }, .tv = { .p = 0, .zl = 0, .zr = 0, .f = X1},    .l =  6, .ptr = &tv_u16_1,   .es = "    34"              },      //    8
    { .rep = { .vs = VAL_SIZE_16, .s = 0 }, .tv = { .p = 0, .zl = 0, .zr = 0, .f = X1},    .l =  6, .ptr = &tv_u16_2,   .es = " 54321"              },      //    9

    { .rep = { .vs = VAL_SIZE_32, .s = 1 }, .tv = { .p = 0, .zl = 0, .zr = 0, .f = X1},    .l = 10, .ptr = &tv_s32_p,   .es = "   1234536"          },      //   10
    { .rep = { .vs = VAL_SIZE_32, .s = 1 }, .tv = { .p = 0, .zl = 0, .zr = 0, .f = X1},    .l = 10, .ptr = &tv_s32_m,   .es = "-    12356"          },      //   11
    { .rep = { .vs = VAL_SIZE_32, .s = 0 }, .tv = { .p = 0, .zl = 0, .zr = 0, .f = X1},    .l = 10, .ptr = &tv_u32_1,   .es = "      3456"          },      //   12
    { .rep = { .vs = VAL_SIZE_32, .s = 0 }, .tv = { .p = 0, .zl = 0, .zr = 0, .f = X1},    .l = 10, .ptr = &tv_u32_2,   .es = "4294967290"          },      //   13

    { .rep = { .vs = VAL_SIZE_32, .s = 1 }, .tv = { .p = 0, .zl = 1, .zr = 0, .f = X1},    .l = 10, .ptr = &tv_s32_p,   .es = "0001234536"          },      //   14
    { .rep = { .vs = VAL_SIZE_32, .s = 1 }, .tv = { .p = 0, .zl = 1, .zr = 0, .f = X1},    .l = 10, .ptr = &tv_s32_m,   .es = "-000012356"          },      //   15
    { .rep = { .vs = VAL_SIZE_32, .s = 0 }, .tv = { .p = 0, .zl = 1, .zr = 0, .f = X1},    .l = 10, .ptr = &tv_u32_1,   .es = "0000003456"          },      //   16
    { .rep = { .vs = VAL_SIZE_32, .s = 0 }, .tv = { .p = 0, .zl = 1, .zr = 0, .f = X1},    .l = 10, .ptr = &tv_u32_2,   .es = "4294967290"          },      //   17

    { .rep = { .vs = VAL_SIZE_8,  .s = 0 }, .tv = { .p = 0, .zl = 1, .zr = 0, .f = X1},    .l = 12, .ptr = &tv_u8_2,    .es = "000000000243"        },      //   18

    { .rep = { .vs = VAL_SIZE_32, .s = 0 }, .tv = { .p = 0, .zl = 0, .zr = 0, .f = X1},    .l =  7, .ptr = &tv_u32_2,   .es = "@967290"             },      //   19

    { .rep = { .vs = VAL_SIZE_32, .s = 0 }, .tv = { .p = 3, .zl = 0, .zr = 0, .f = X1000}, .l = 10, .ptr = &tv_s32_p,   .es = "  1234.536"          },      //   20
    { .rep = { .vs = VAL_SIZE_32, .s = 0 }, .tv = { .p = 1, .zl = 0, .zr = 0, .f = X1000}, .l = 10, .ptr = &tv_s32_p,   .es = "    1234.5"          },      //   21
    { .rep = { .vs = VAL_SIZE_32, .s = 0 }, .tv = { .p = 3, .zl = 0, .zr = 0, .f = X10},   .l = 10, .ptr = &tv_s32_p,   .es = "123453.6  "          },      //   22
    { .rep = { .vs = VAL_SIZE_32, .s = 0 }, .tv = { .p = 3, .zl = 0, .zr = 1, .f = X10},   .l = 10, .ptr = &tv_s32_p,   .es = "123453.600"          },      //   23
    { .rep = { .vs = VAL_SIZE_32, .s = 0 }, .tv = { .p = 3, .zl = 0, .zr = 0, .f = X1000}, .l = 10, .ptr = &tv_u32_p,   .es = "  9765.4  "          },      //   24
    { .rep = { .vs = VAL_SIZE_32, .s = 0 }, .tv = { .p = 2, .zl = 0, .zr = 0, .f = X1000}, .l = 10, .ptr = &tv_u32_p,   .es = "   9765.4 "          },      //   25

    { .rep = { .vs = VAL_SIZE_32, .s = 0 }, .tv = { .p = 3, .zl = 0, .zr = 0, .f = X100},  .l = 10, .ptr = &tv_u32_p,   .es = " 97654.0  "          },      //   26
    { .rep = { .vs = VAL_SIZE_32, .s = 0 }, .tv = { .p = 3, .zl = 0, .zr = 0, .f = X1},    .l = 10, .ptr = &tv_u32_1,   .es = "  3456.0  "          },      //   27

    { .rep = { .vs = VAL_SIZE_8,  .s = 0 }, .tv = { .p = 3, .zl = 0, .zr = 0, .f = X1000}, .l = 10, .ptr = &tv_u8_1,    .es = "     0.003"          },      //   27
    { .rep = { .vs = VAL_SIZE_8,  .s = 1 }, .tv = { .p = 3, .zl = 0, .zr = 0, .f = X1000}, .l = 10, .ptr = &tv_s8_m,    .es = "-    0.012"          },      //   29
    { .rep = { .vs = VAL_SIZE_32, .s = 1 }, .tv = { .p = 3, .zl = 0, .zr = 0, .f = X1000}, .l = 10, .ptr = &tv_u32_0,   .es = "     0.0  "          },      //   30
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
        unsigned exp_len = str_len(tests[i].es, 20);

        val_text_ptr_to_str(str, tests[i].ptr, tests[i].rep, tests[i].tv, tests[i].l);

        unsigned res_len = str_len(str, 20);
        if (res_len != exp_len) {
            res = 1;
        }
        if (str_cmp(str, tests[i].es, exp_len) == 0) {
            res = 1;
        }

        printf("  TEST: %4d         [%2d] %20s<     expect: [%2d] %20s<    %s\r\n", i + 1, res_len, str, exp_len, tests[i].es, res_str[res]);

        if (res) {
            total_res = 1;
            bad_cnt++;
        }
    }
    printf("total result: %s bad test %d\n", res_str[total_res], bad_cnt);
    return total_res;
}
