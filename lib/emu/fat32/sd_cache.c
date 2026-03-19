#include "sd_cache.h"
#include <stdio.h>
#include "sd_card.h"

static uint8_t buf[SD_SECTOR_SIZE];
unsigned loaded_sector = -1;

uint8_t * sector_load(unsigned sector)
{
    if (sector == loaded_sector) {
        return;
    }
    FILE * f = fopen("sd.img", "r+");
    fseek(f, (long)sector * SD_SECTOR_SIZE, SEEK_SET);
    fread(buf, 1, SD_SECTOR_SIZE, f);
    fclose(f);
    loaded_sector = sector;
    return buf;
}

void sector_mark_dirty(unsigned sector)
{
}
