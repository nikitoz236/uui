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

void dump(uint8_t * ptr, uint32_t address, unsigned len)
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

void sector_dump(unsigned sector, unsigned len)
{
    while (len--) {
        uint8_t * ptr = sector_load(sector);
        dump(ptr, sector * 512, 512);
        sector++;
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
        dp("  sectors_per_cluster: "); dpd(fat.sectors_per_cluster, 1); dn();
        dp("  root_dir_cl:         "); dpd(fat.root_dir_cl, 1); dn();
        dp("  sector_of_zero_cl:   "); dpd(fat.sector_of_zero_cl, 4); dn();
    }

    // dpn("fat:");
    // sector_dump(fat.fat_offset[0], 2);

    dpn("root dir:");
    // sector_dump(sector_of_cluster(&fat, fat.root_dir_cl), 12);

    unsigned fr = 0;
    unsigned r;
    uint8_t buf[1024];
    while (r = dir_scan(&fat, fat.root_dir_cl, fr, (utf16_t *)buf, 512)) {
        dp("read dir entry num "); dpd(r, 2);
        // dp(" name: "); dpxd(buf, 2, 40); dn();

        unsigned l = utf16_to_utf8(buf, 1024, (utf16_t *)buf, 0);
        dp(" str len: "); dpd(l, 5); dp(" name !!! : "); dp(buf); dn(); dn();
        fr += r;
    }

    return 0;
}
