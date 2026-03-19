#include <stdio.h>
#include <stdint.h>
#include "utf.h"
#include "str_utils.h"
#include "array_size.h"
#include "dp.h"

struct {
    utf16_t * s;
    unsigned sl;
    unsigned dl;
    utf8_t * ref;
    unsigned ref_len;
} tests[] = {
    { .s = (utf16_t []){ 'h', 'e', 'y' }, .sl = 2, .dl = 10, .ref = "he", .ref_len = 2 },
    { .s = (utf16_t []){ 'h', 'e', 'y', 0 }, .sl = 0, .dl = 10, .ref = "hey", .ref_len = 3 },
    { .s = (utf16_t []){ 'h', 'e', 'y', 0 }, .sl = 0, .dl = 2, .ref = "hey", .ref_len = 2 },
};

unsigned do_test(utf16_t * s, unsigned sl, unsigned dl, utf8_t * ref, unsigned ref_len)
{
    unsigned ret = 0;
    utf8_t res[1024];
    unsigned l_res = utf16_to_utf8(res, dl, s, sl);
    if (l_res != ref_len) {
        dpn("returned len != ref len");
        ret = 1;
    }
    if (!str_cmp(res, ref, ref_len)) {
        dpn("str not equal");
        ret = 1;
    }
    return ret;
}

int main() {
    dpn("utf test");
    char *rs[] = { "OK", "FAIL" };
    unsigned result = 0;
    for ARRAY_INDEX(i, tests) {
        dp("TEST "); dpd(i, 3); dn();
        unsigned r = do_test(tests[i].s, tests[i].sl, tests[i].dl, tests[i].ref, tests[i].ref_len);
        if (r) {
            result = 1;
        }
        dpn(rs[r]);
    }
    dp("TEST "); dpn(rs[result]);

    return result;
}
