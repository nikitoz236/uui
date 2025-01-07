#include <stdio.h>
#include "array_size.h"
#include "forms.h"
#include "forms_align.h"

struct {
    form_t pf;
    xy_t s;
    align_t a;
    xy_t margin;
    xy_t res;
} tests[] = {
    { .pf = { .p = { .x = 10, .y = 20 }, .s = { .w = 30, .h = 40 } }, .s = { .w = 10, .h = 20 }, .a = { .x = { .center = 1 }, .y = { .center = 1 } }, .margin = { .x = 1, .y = 2 }, .res = { .x = 20, .y = 30 } },
    { .pf = { .p = { .x = 10, .y = 20 }, .s = { .w = 30, .h = 40 } }, .s = { .w = 10, .h = 20 }, .a = { .x = { .edge = EDGE_L, .side = ALIGN_I, .center = 0 }, .y = { .edge = EDGE_D, .side = ALIGN_I, .center = 0 } }, .margin = { .x = 4, .y = 8 }, .res = { .x = 14, .y = 20 + 40 - 8 - 20 } },
    { .pf = { .p = { .x = 100, .y = 20 }, .s = { .w = 30, .h = 40 } }, .s = { .w = 10, .h = 20 }, .a = { .x = { .edge = EDGE_L, .side = ALIGN_O, .center = 0 }, .y = { .edge = EDGE_D, .side = ALIGN_O, .center = 0 } }, .margin = { .x = 8, .y = 12 }, .res = { .x = 100 - 8 - 10, .y = 20 + 40 + 12 } },
};

int main()
{
    printf("test forms align\n");
    unsigned total_res = 0;
    unsigned bad_cnt = 0;
    const char * res_str[] = { "OK", "FAIL" };
    for (unsigned i = 0; i < ARRAY_SIZE(tests); i++) {
        unsigned res = 0;

        const char edge[][2] = {{ [EDGE_L] = 'L', [EDGE_R] = 'R' }, { [EDGE_U] = 'U', [EDGE_D] = 'D' } };
        const char side[] = { [ALIGN_I] = 'I', [ALIGN_O] = 'O' };
        char astr[2][2] = {0};

        xy_t res_pos = align_form_pos(&tests[i].pf, tests[i].s, &tests[i].a, &tests[i].margin);

        for (unsigned d = 0; d < DIMENSION_COUNT; d++) {
            if (res_pos.ca[d] != tests[i].res.ca[d]) {
                res = 1;
            }
            if (tests[i].a.ca[d].center) {
                astr[d][0] = ' ';
                astr[d][1] = 'C';
            } else {
                astr[d][0] = edge[d][tests[i].a.ca[d].edge];
                astr[d][1] = side[tests[i].a.ca[d].side];
            }
        }

        printf("  TEST: %4d         pf: (%3d, %3d)[%3d, %3d], align size: [%3d, %3d], align: (%c%c, %c%c), margin: [%3d, %3d], rp(%3d, %3d), ep(%3d, %3d)      %s\r\n",
            i + 1,
            tests[i].pf.p.x, tests[i].pf.p.y, tests[i].pf.s.w, tests[i].pf.s.h,
            tests[i].s.w, tests[i].s.h,
            astr[0][0], astr[0][1], astr[1][0], astr[1][1],
            tests[i].margin.x, tests[i].margin.y,
            res_pos.x, res_pos.y,
            tests[i].res.x, tests[i].res.y,
            res_str[res]
        );

        if (res) {
            total_res = 1;
            bad_cnt++;
        }
    }
    printf("total result: %s bad test %d\n", res_str[total_res], bad_cnt);
    return total_res;
}
