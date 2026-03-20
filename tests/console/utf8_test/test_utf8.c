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
    // ASCII
    // 1
    { .s = (utf16_t []){ 'h', 'e', 'y' }, .sl = 2, .dl = 10, .ref = "he", .ref_len = 2 },
    // 2
    { .s = (utf16_t []){ 'h', 'e', 'y', 0 }, .sl = 0, .dl = 10, .ref = "hey", .ref_len = 4 },
    // 3
    { .s = (utf16_t []){ 'h', 'e', 'y', 0 }, .sl = 0, .dl = 2, .ref = "hey", .ref_len = 2 },
    // 4 - Русские буквы (кириллица) "Привет"
    { .s = (utf16_t []){ 0x041F, 0x0440, 0x0438, 0x0432, 0x0435, 0x0442, 0 }, .sl = 0, .dl = 16, .ref = "\xD0\x9F\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82", .ref_len = 13 },
    // 5 - Русское "ёж" + latin "abc"
    { .s = (utf16_t []){ 0x0451, 0x0436, 'a', 'b', 'c', 0 }, .sl = 0, .dl = 16, .ref = "\xD1\x91\xD0\xB6""abc", .ref_len = 8 },
    // 6 - Суррогатная пара (high surrogate)
    { .s = (utf16_t []){ 0xD834, 0xDD1E, 0 }, .sl = 0, .dl = 8, .ref = "@@", .ref_len = 3 },
    // 7 - Одиночный суррогат (low surrogate)
    { .s = (utf16_t []){ 0xDD1E, 0 }, .sl = 0, .dl = 8, .ref = "@", .ref_len = 2 },
};

unsigned do_test(utf16_t * s, unsigned sl, unsigned dl, utf8_t * ref, unsigned ref_len)
{
    unsigned ret = 0;
    utf8_t res[1024];
    unsigned l_res = utf16_to_utf8(res, dl, s, sl);
    if (l_res != ref_len) {
        dp("returned len "); dpd(l_res, 3); dp(" != ref len "); dpd(ref_len, 3); dn();
        ret = 1;
    }
    if (!str_cmp(res, ref, ref_len)) {
        dpn("str not equal");
        dp("res: "); dpxd(res, 1, l_res); dn();
        dp("ref: "); dpxd(ref, 1, ref_len); dn();
        ret = 1;
    }
    return ret;
}

int main() {
    dpn("utf test");
    char *rs[] = { "OK", "FAIL" };
    unsigned result = 0;
    for ARRAY_INDEX(i, tests) {
        dn();
        dp("TEST "); dpd(i + 1, 3); dn();
        unsigned r = do_test(tests[i].s, tests[i].sl, tests[i].dl, tests[i].ref, tests[i].ref_len);
        if (r) {
            result = 1;
        }
        dpn(rs[r]);
    }
    dp("TEST "); dpn(rs[result]);

    return result;
}
