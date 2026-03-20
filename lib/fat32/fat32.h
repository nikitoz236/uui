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
    uint32_t cluster;
    uint32_t size;
    uint16_t folder_record;
    uint8_t num_records;
} fat32_file_record_t;

/*
    итак я хочу фукнцию которая будет брать номер кластера с каталогом, а также номер записи файла. читать имя и количество записей на этот файл в каталоге. 
*/
unsigned dir_scan(const fat32_t * fat, uint32_t dir_cluster, unsigned frn, char * name, unsigned max_name_len);
