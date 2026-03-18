#include <stdint.h>

typedef struct {
    uint32_t fat_offset[2];
    uint32_t root_dir;
    uint16_t sector_size;
    uint8_t sectors_per_cluster;
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
unsigned fat32_get_file_record();
