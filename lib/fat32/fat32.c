#include "fat32.h"
#include "sd_cache.h"

#include "stddef.h"
#include "array_size.h"
#include "buf_endian.h"
#include "str_utils.h"

#define DP_OFF
#include "dp.h"

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

typedef struct __attribute__((packed)) {
    uint8_t  jmp[3];                    // Первые 3 байта: JMP-инструкция — код для перехода на загрузчик (Boot Jump Instruction)
    uint8_t  oem[8];                    // ASCII-строка (OEM Name) — производитель или идентификатор FAT (например, "MSDOS5.0")
    uint16_t bytes_per_sector;          // Размер сектора в байтах (обычно 512, 1024, 2048 или 4096)
    uint8_t  sectors_per_cluster;       // Количество секторов в одном кластере
    uint16_t reserved_sectors;          // Количество зарезервированных секторов перед областью FAT (всегда >= 1)
    uint8_t  num_fats;                  // Количество копий FAT (обычно 2)
    uint16_t root_entry_count;          // Количество корневых записей (0 для FAT32; >0 только для FAT12/16)
    uint16_t total_sectors_16;          // Общее число секторов (если >65535, то в total_sectors_32)
    uint8_t  media_type;                // Тип носителя (Media Descriptor, 0xF8 = HDD)
    uint16_t fat_size_16;               // Размер одной FAT в секторах (0 для FAT32)
    uint16_t sectors_per_track;         // Количество секторов на дорожке (для CHS-адресации BIOS, не используется в FAT32)
    uint16_t num_heads;                 // Количество головок (CHS, не используется в FAT32)
    uint32_t hidden_sectors;            // Количество "скрытых" секторов до начала раздела (важно для MBR)
    uint32_t total_sectors_32;          // Общее количество секторов (если total_sectors_16 == 0, то используем это поле)
    uint32_t fat_size_32;               // Размер одной таблицы FAT в секторах (для FAT32)
    uint16_t ext_flags;                 // Расширенные флаги FAT32 (например, информация о синхронизации FAT, активной копии и др.)
    uint16_t fs_version;                // Версия файловой системы (почти всегда 0)
    uint32_t root_cluster;              // Номер первого кластера корневой директории (обычно 2)
} fat32_boot_record_t;

enum {
    FR_ATTR_READ_ONLY = 0x01,           // Файл только для чтения
    FR_ATTR_HIDDEN    = 0x02,           // Скрытый файл
    FR_ATTR_SYSTEM    = 0x04,           // Системный файл
    FR_ATTR_VOLUME_ID = 0x08,           // Элемент - метка тома
    FR_ATTR_DIRECTORY = 0x10,           // Элемент - папка (директория)
    FR_ATTR_ARCHIVE   = 0x20,           // Архивный файл (для резервных копий)
    /**
     * Атрибут LFN для записи длинного имени файла в каталоге.
     * Представляет собой битовую маску (ReadOnly | Hidden | System | VolumeID == 0x0F).
     * Используется только для LFN-записей, не относится к обычным файлам/папкам.
     */
    FR_ATTR_LFN       = 0x0F
};

typedef struct __attribute__((packed)) {
    uint8_t order;                      // Порядковый номер LFN-записи. Последняя содержит 0x40.
    uint16_t name1[5];                  // Первые 5 символов имени (UTF-16).
    uint8_t attr;                       // Должен быть 0x0F для LFN.
    uint8_t type;                       // Всегда 0x00 для LFN.
    uint8_t checksum;                   // Контрольная сумма соответствующей MBS-записи (SFN).
    uint16_t name2[6];                  // Следующие 6 символов имени (UTF-16).
    uint16_t zero;                      // Должен быть 0x0000.
    uint16_t name3[2];                  // Последние 2 символа имени (UTF-16).
} fat32_lfn_entry_t;

#define LFN_CHARS                       (5 + 6 + 2)

typedef struct __attribute__((packed)) {
    uint16_t day   : 5;                 // [0:4] 1–31
    uint16_t month : 4;                 // [5:8] 1–12
    uint16_t year  : 7;                 // [9:15] years since 1980
} fat32_date_t;

typedef struct __attribute__((packed)) {
    uint16_t seconds : 5;               // [0:4] 0–29 (умножить на 2; реально 0–58 sec)
    uint16_t minutes : 6;               // [5:10] 0–59
    uint16_t hours   : 5;               // [11:15] 0–23
} fat32_time_t;

typedef struct __attribute__((packed)) {
    uint8_t name[8];                    // Имя файла в формате 8 символов (ASCII, без точки, дополняется пробелами)
    uint8_t ext[3];                     // Расширение файла: 3 символа (ASCII, дополняется пробелами)
    union {
        uint8_t attr;                       // Атрибуты файла (биты: ReadOnly, Hidden, System, VolumeID, Directory, Archive)
        struct {
            uint8_t attr_read_only : 1;
            uint8_t attr_hidden : 1;
            uint8_t attr_system : 1;
            uint8_t attr_volume : 1;
            uint8_t attr_directroy : 1;
        };
    };
    uint8_t nt_reserved;                // Зарезервировано Windows NT (исп. для корректировки регистра символов имени)
    uint8_t creation_time_tenths;       // Доля секунды создания (0-199, кратно 10 мс; точность FAT - 2 сек)
    fat32_time_t creation_time;         // Время создания файла (битовое поле: часы, минуты, секунды/2)
    fat32_date_t creation_date;         // Дата создания файла (битовое поле: день, месяц, год-1980)
    fat32_date_t last_access_date;      // Дата последнего доступа (без времени; битовое поле)
    uint16_t first_cluster_high;        // Старшие 16 бит номера первого кластера файла (только для FAT32)
    fat32_time_t write_time;            // Время последней записи/изменения (битовое поле)
    fat32_date_t write_date;            // Дата последней записи/изменения (битовое поле)
    uint16_t first_cluster_low;         // Младшие 16 бит номера первого кластера файла
    uint32_t file_size;                 // Размер файла в байтах
} fat32_sfn_entry_t;

typedef union {
    fat32_lfn_entry_t lfn;
    fat32_sfn_entry_t sfn;
} fat32_dir_entry_t;

// Значения кластеров в FAT32
#define FAT32_CLUSTER_FREE         0x0000000  // Свободный кластер
#define FAT32_CLUSTER_RESERVED     0x0000001  // Зарезервировано (недопустимо)
#define FAT32_CLUSTER_FIRST        0x0000002  // Первый валидный кластер
#define FAT32_CLUSTER_MAX          0xFFFFFEF  // Максимальный валидный кластер
#define FAT32_CLUSTER_BAD          0xFFFFFF7  // Bad cluster — битый, не использовать
#define FAT32_CLUSTER_EOC_MIN      0xFFFFFF8  // Минимальное EOC-значение (конец цепи)

#define FILE_RECORDS_PER_SECTOR     (512 / sizeof(fat32_dir_entry_t))
#define FAT_CLUSTERS_PER_SECTOR     (512 / sizeof(uint32_t))


unsigned init_fat32(fat32_t * fat32)
{
    boot_sector_t * mbr = (boot_sector_t *)sector_load(0);
    if (mbr->signature != 0xAA55) {
        return 0;
    }

    unsigned part_start = 0;
    fat32_boot_record_t * bpb = (fat32_boot_record_t *)mbr;

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
        bpb = (fat32_boot_record_t *)sector_load(part_start);
        if (bpb->jmp[0] != 0xEB && bpb->jmp[0] != 0xE9) {
            return 0;
        }
    }

    fat32->fat_offset[0] = part_start + bpb->reserved_sectors;
    fat32->fat_offset[1] = part_start + bpb->reserved_sectors + bpb->fat_size_32;

    fat32->root_dir_cl = bpb->root_cluster;
    fat32->sectors_per_cluster = bpb->sectors_per_cluster;

    fat32->sector_of_zero_cl = fat32->fat_offset[1] + bpb->fat_size_32 - (2 * bpb->sectors_per_cluster);

    return 1;
}

static unsigned sector_of_cluster(const fat32_t * fat, uint32_t cluster, unsigned sector_in_cluster)
{
    return fat->sector_of_zero_cl + (cluster * fat->sectors_per_cluster) + sector_in_cluster;
}

static uint32_t fat32_next_cluster(const fat32_t * fat, uint32_t cluster)
{
    uint32_t sector = fat->fat_offset[0] + cluster / FAT_CLUSTERS_PER_SECTOR;
    uint32_t offset = cluster % FAT_CLUSTERS_PER_SECTOR;

    uint32_t * table = (uint32_t *)sector_load(sector);
    return table[offset];
}

static unsigned name_from_lfn(const fat32_lfn_entry_t * lfn, utf16_t * name, unsigned max_len)
{
    static const struct { uint8_t o; uint8_t l; } offsets[] = {
        { .o = offsetof(fat32_lfn_entry_t, name1), .l = 5 },
        { .o = offsetof(fat32_lfn_entry_t, name2), .l = 6 },
        { .o = offsetof(fat32_lfn_entry_t, name3), .l = 2 }
    };

    unsigned total_used = 0;
    uint8_t * ptr = (uint8_t *)lfn;
    for ARRAY_INDEX(i, offsets) {
        for (unsigned j = 0; j < offsets[i].l; j++) {
            if (max_len == 0) {
                return total_used;
            }
            // dp("lfn decode: "); dpd(total_used, 2);
            utf16_t c_u16 = u16_from_le_buf8(&ptr[offsets[i].o + (2 * j)]);
            // dp(" char: "); dpx(c_u16, 2); dn();
            name[total_used++] = c_u16;
            max_len--;
            if (c_u16 == 0) {
                // dp("lfn detect 0: "); dpd(total_used, 2); dn();
                return total_used;
            }
        }
    }
    return total_used;
}

static unsigned name_form_sfn(const fat32_sfn_entry_t * sfn, char * name)
{
    str_cp(&name[0], sfn->name, 8);
    name[8] = '.';
    str_cp(&name[9], sfn->ext, 3);
    name[9 + 3] = 0;
    return 8 + 1 + 3 + 1;
}

static unsigned sector_of_file(const fat32_t * fat, uint32_t cluster, unsigned sector)
{
    while (sector >= fat->sectors_per_cluster) {
        cluster = fat32_next_cluster(fat, cluster);
        if (cluster >= FAT32_CLUSTER_EOC_MIN) {
            return 0;
        }
        if (cluster == FAT32_CLUSTER_BAD) {
            return 0;
        }
        sector -= fat->sectors_per_cluster;
    }
    return sector_of_cluster(fat, cluster, sector);
}

static fat32_dir_entry_t * get_dir_entry(const fat32_t * fat, uint32_t dir_cluster, unsigned frn)
{
    dp("dir "); dpd(dir_cluster, 10); dp(" scan, record "); dpd(frn, 5); dn();

    unsigned disk_sector = sector_of_file(fat, dir_cluster, frn / FILE_RECORDS_PER_SECTOR);
    if (disk_sector == 0) {
        return 0;
    }

    fat32_dir_entry_t * dir_sector = (fat32_dir_entry_t *)sector_load(disk_sector);
    fat32_dir_entry_t * entry = &dir_sector[frn % FILE_RECORDS_PER_SECTOR];

    dp("entry: "); dpx((unsigned)entry, 4); dp(" : "); dpxd(entry, 1, sizeof(fat32_dir_entry_t)); dn();

    if (entry->lfn.order == 0) {
        return 0;
    }

    return entry;
}

unsigned dir_scan(const fat32_t * fat, uint32_t dir_cluster, unsigned frn, char * name, unsigned max_name_len, fat32_file_record_t * f)
{
    unsigned records = 0;
    f->num_records = 0;
    unsigned name_len = 0;

    dn();

    while (1) {
        records++;
        fat32_dir_entry_t * entry = get_dir_entry(fat, dir_cluster, frn++);

        if (entry == 0) {
            return 0;
        }

        if (entry->sfn.name[0] == 0xE5) {
            dpn("-- skip E5");
            continue;
        }
        if (entry->sfn.name[0] == 0xFF) {
            dpn("-- skip FF");
            continue;
        }

        f->num_records++;
        if (entry->lfn.attr == FR_ATTR_LFN) {
            dp("LFN ");
            unsigned order = entry->lfn.order;
            if (order & 0x40) {
                f->folder_record = frn;
                order &= ~0x40;
            }
            dpd(order, 3);
            utf16_t * name_part = (utf16_t *)&name[sizeof(utf16_t) * LFN_CHARS * (order - 1)];
            unsigned l = name_from_lfn(&entry->lfn, name_part, max_name_len / sizeof(utf16_t));
            name_len += l;
            dp(" lfn len: "); dpd(l, 2); dp(" name: "); dpxd(name_part, 2, 13); dn();
        } else {
            dp("SFN ");
            if (name_len == 0) {
                f->name_len = name_form_sfn(&entry->sfn, name);
            } else {
                utf16_t * n16 = (utf16_t *)name;
                if (n16[name_len - 1] != 0) {
                    n16[name_len] = 0;
                    name_len++;
                }
                f->name_len = utf16_to_utf8(name, max_name_len, n16, 0);
            }
            f->cluster = entry->sfn.first_cluster_high << 16;
            f->cluster += entry->sfn.first_cluster_low;

            f->size = entry->sfn.file_size;
            f->name = name;
            f->folder_cluster = dir_cluster;
            f->folder = entry->sfn.attr_directroy;
            break;
        }
    }

    return records;
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
