#include <stdint.h>
#include "utf.h"

typedef struct {
    uint32_t fat_offset[2];
    uint32_t sector_of_zero_cl;
    uint16_t sector_size;
    uint8_t sectors_per_cluster;
    uint8_t root_dir_cl;
} fat32_t;

unsigned init_fat32(fat32_t * fat32);

typedef struct {
    uint32_t cluster;                   // кластер файла
    uint32_t folder_cluster;            // кластер в котором находится директория содержащая файл
    uint32_t size;                      // размер файла
    utf8_t * name;                      // имя файла в utf8
    uint16_t folder_record;             // номер записи в каталоге с которого начинается LFN
    uint8_t name_len;                   // длина имени файла в байтах utf8
    uint8_t num_records : 4;            // количество записей в каталоге на файл
    uint8_t folder : 1;
} fat32_file_record_t;

unsigned dir_scan(const fat32_t * fat, uint32_t dir_cluster, unsigned frn, char * name, unsigned max_name_len, fat32_file_record_t * f);
unsigned file_by_path(const fat32_t * fat, fat32_file_record_t * f, const char * path);
