#include <stdio.h>

#define DP_NOTABLE
#include "dp.h"

#include "timers_32.h"

struct tc {
    uint32_t ct;            // с каким значением времени сравнить
    unsigned is_ovf;        // ожидаемое значение over
    unsigned is_act;        // ожидаемое значение actime
    unsigned remain;        // ожидаемое значение remain
};

unsigned do_tc(struct tc * tc, timer_32_t * t)
{
    unsigned is_ovf = t32_is_over(t, tc->ct);
    unsigned is_act = t32_is_active(t, tc->ct);
    unsigned remain = t32_remain(t, tc->ct);
    dp("test t32 ["); dpx(t->val, 4); dp("] t = "); dpx(t->time, 4); dp("a = "); dpd(t->active, 1); dp(" with time "); dpx(tc->ct, 4); dp(" : is over = "); dpd(is_ovf, 1); dp(" : is active = "); dpd(is_act, 1); dp(" : remain = "); dpd(remain, 4);
    // dn();
    if (is_ovf == tc->is_ovf) {
        if (is_act == tc->is_act) {
            if (remain == tc->remain) {
                dp("        - OK"); dn();
                return 1;
            }
        }
    }
    dp("        - FAIL"); dn();
    return 1;
}

int main()
{
    dp("test timers 32\n");

    unsigned res = 1;

    timer_32_t t = {};

    t32_run(&t, 0x80000000, 50);

    // проверка только с половиной периода основного счетчика, цикл 2 ** 31, старший бит не значащий
    res &= do_tc(&(struct tc){ .ct = 0x00000031, .is_ovf = 0, .is_act = 1, .remain = 1 }, &t);
    res &= do_tc(&(struct tc){ .ct = 0x80000030, .is_ovf = 0, .is_act = 1, .remain = 2 }, &t);
    res &= do_tc(&(struct tc){ .ct = 0x00000040, .is_ovf = 1, .is_act = 0, .remain = 0 }, &t);

    t32_stop(&t);

    res &= do_tc(&(struct tc){ .ct = 0x80000030, .is_ovf = 0, .is_act = 0, .remain = 0 }, &t);

    t32_extend(&t, 10);

    res &= do_tc(&(struct tc){ .ct = 0x80000032 + 2, .is_ovf = 0, .is_act = 1, .remain = 8 }, &t);

    // проверка с переполнением значения таймера
    t32_run(&t, 0xFFFFFFF0, 50);

    res &= do_tc(&(struct tc){ .ct = 0xFFFFFFF0, .is_ovf = 0, .is_act = 1, .remain = 50 }, &t);
    res &= do_tc(&(struct tc){ .ct = 0xFFFFFFFF, .is_ovf = 0, .is_act = 1, .remain = 50 - 15 }, &t);

    // проверка с переполнением основного счетчика
    t32_run(&t, 0xFFFFFFF0, 10);

    res &= do_tc(&(struct tc){ .ct = 0xFFFFFFFF, .is_ovf = 1, .is_act = 0, .remain = 0 }, &t);
    res &= do_tc(&(struct tc){ .ct = 0, .is_ovf = 1, .is_act = 0, .remain = 0 }, &t);
    res &= do_tc(&(struct tc){ .ct = 0x80000000, .is_ovf = 1, .is_act = 0, .remain = 0 }, &t);
    res &= do_tc(&(struct tc){ .ct = 0x80000005, .is_ovf = 1, .is_act = 0, .remain = 0 }, &t);

    t32_extend(&t, 10);

    res &= do_tc(&(struct tc){ .ct = 0xFFFFFFFF, .is_ovf = 0, .is_act = 1, .remain = 5 }, &t);
    res &= do_tc(&(struct tc){ .ct = 0, .is_ovf = 0, .is_act = 1, .remain = 4 }, &t);
    res &= do_tc(&(struct tc){ .ct = 0x80000000, .is_ovf = 0, .is_act = 1, .remain = 4 }, &t);
    res &= do_tc(&(struct tc){ .ct = 0x80000005, .is_ovf = 1, .is_act = 0, .remain = 0 }, &t);


    char * res_str[] = { "FAIL", "OK" };
    dp("Final result: "); dpn(res_str[res]);

    return 0;
}
