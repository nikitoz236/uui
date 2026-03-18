#include <stdio.h>

#include "dp.h"

unsigned is_char(char c)
{
    if (c < ' ') {
        return 0;
    }
    if (c > '~') {
        return 0;
    }
    return 1;
}

void sector_dump(uint8_t * ptr, uint32_t address, unsigned len)
{
    while (len) {
        dpx(address, 4); dp(": ");
        unsigned sl = 16;
        if (len < 16) {
            sl = len;
        }

        dpxd(ptr, 1, sl);
        for(unsigned i = sl; i < 16; i++) {
            dp("   ");
        }

        dp("  ");

        for (unsigned i = 0; i < sl; i++) {
            if (is_char(ptr[i])) {
                dpl(&ptr[i], 1);
            } else {
                dp(".");
            }
        }

        dn();

        len -= sl;
        ptr += sl;
        address += sl;
    }
}

int main()
{
    char str[] = "test fat32";
    dpn(str);

    sector_dump((uint8_t *)str, 0xabcd0030, 23);

    return 0;
}
