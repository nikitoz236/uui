#include <stdio.h>

#include "dp.h"
#include "sd_cache.h"
#include "fat32.h"
#include "str_utils.h"

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

unsigned file_by_path(const fat32_t * fat, fat32_file_record_t * f, const char * path)
{
    uint32_t current_cl = fat->root_dir_cl;
    char name_buf[256];

    while (1) {
        while (*path == '/') {
            path++;
        }
        if (*path == 0) {
            return 0;
        }

        unsigned comp_len = str_find(path, str_len(path, 256), '/');

        unsigned frn = 0;
        unsigned r;
        while ((r = dir_scan(fat, current_cl, frn, name_buf, sizeof(name_buf), f))) {
            unsigned nlen = str_len(name_buf, sizeof(name_buf));
            if (nlen == comp_len && str_cmp(name_buf, path, comp_len)) {
                path += comp_len;
                while (*path == '/') {
                    path++;
                }
                if (*path == 0) {
                    f->name = 0;
                    return 1;
                }
                if (!f->folder) {
                    return 0;
                }
                current_cl = f->cluster;
                break;
            }
            frn += r;
        }
        if (!r) {
            return 0;
        }
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
    uint32_t folder = fat.root_dir_cl;
    // uint32_t folder = 3;
    fat32_file_record_t f;
    uint8_t buf[1024];
    while (r = dir_scan(&fat, folder, fr, buf, 512, &f)) {
        dp("read dir entry num: "); dpd(r, 2); dp(" size: "); dpd(f.size, 10); dp(" cl: "); dpd(f.cluster, 10); dp((char*[]){" F ", " D "}[f.folder]);
        dp(" fr: "); dpd(f.folder_record, 5); dp(" frn: "); dpd(f.num_records, 3); dp(" name : "); dp(buf); dp(" - "); dpxd(buf, 1, 6); dn();

        fr += r;
    }

    // char path[] = "gps/200202_pri/02103104.GPX";
    char path[] = "honda.bmp";

    file_by_path(&fat, &f, path);
        dp("searched path: "); dp(path); dp(" size: "); dpd(f.size, 10); dp(" cl: "); dpd(f.cluster, 10); dp((char*[]){" F ", " D "}[f.folder]);
        dp(" fr: "); dpd(f.folder_record, 5); dp(" frn: "); dpd(f.num_records, 3); dp(" name : "); dp(buf); dp(" - "); dpxd(buf, 1, 6); dn();

    return 0;
}
