#include <stdio.h>

#include "dp.h"
#include "sd_cache.h"
#include "fat32.h"

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
    dpn("test fat32");

    fat32_t fat;
    if (init_fat32(&fat)) {
        dpn("read fat .... OK");
        dp("  fat_offset[0]:       "); dpd(fat.fat_offset[0], 4); dn();
        dp("  fat_offset[1]:       "); dpd(fat.fat_offset[1], 4); dn();
        dp("  root_dir:            "); dpd(fat.root_dir, 4); dn();
        dp("  sectors_per_cluster: "); dpd(fat.sectors_per_cluster, 1); dn();
    }

    for (unsigned i = 0; i < 4; i++) {
        uint8_t * sector = sector_load(i + fat.root_dir);
        sector_dump(sector, i + (fat.root_dir) * 512, 512);
    }

    return 0;
}
