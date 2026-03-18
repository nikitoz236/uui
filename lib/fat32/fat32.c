#include "fat32.h"
#include "sd_cache.h"

typedef struct __attribute__((packed)) {
    uint8_t jmp[3];
    uint8_t oem[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t num_fats;
    uint16_t root_entry_count;
    uint16_t total_sectors_16;
    uint8_t media_type;
    uint16_t fat_size_16;
    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
    uint32_t fat_size_32;
    uint16_t ext_flags;
    uint16_t fs_version;
    uint32_t root_cluster;
} fat_boot_record_t;

typedef struct __attribute__((packed)) {
    uint8_t status;
    uint8_t chs_first[3];
    uint8_t type;
    uint8_t chs_last[3];
    uint32_t lba_start;
    uint32_t lba_size;
} mbr_part_entry_t;

typedef struct __attribute__((packed)) {
    uint8_t bootstrap[446];
    mbr_part_entry_t partitions[4];
    uint16_t signature;
} boot_sector_t;

unsigned init_fat32(fat32_t * fat32)
{
    boot_sector_t * mbr = (boot_sector_t *)sector_load(0);
    if (mbr->signature != 0xAA55) {
        return 0;
    }

    unsigned part_start = 0;
    fat_boot_record_t * bpb = (fat_boot_record_t *)mbr;

    if (bpb->jmp[0] != 0xEB && bpb->jmp[0] != 0xE9) {
        unsigned i;
        for (i = 0; i < 4; i++) {
            if (mbr->partitions[i].type == 0x0B || mbr->partitions[i].type == 0x0C) {
                part_start = mbr->partitions[i].lba_start;
                break;
            }
        }
        if (i == 4) {
            return 0;
        }
        bpb = (fat_boot_record_t *)sector_load(part_start);
        if (bpb->jmp[0] != 0xEB && bpb->jmp[0] != 0xE9) {
            return 0;
        }
    }

    fat32->fat_offset[0] = part_start + bpb->reserved_sectors;
    fat32->fat_offset[1] = part_start + bpb->reserved_sectors + bpb->fat_size_32;
    unsigned data_start = bpb->reserved_sectors + bpb->num_fats * bpb->fat_size_32;
    fat32->root_dir = part_start + data_start + (bpb->root_cluster - 2) * bpb->sectors_per_cluster;
    fat32->sectors_per_cluster = bpb->sectors_per_cluster;
    return 1;
}
