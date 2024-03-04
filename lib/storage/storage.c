#include "storage.h"
#include "flash_hw.h"
#include "misc.h"
#include "stddef.h"

#define STORAGE_START_PAGE          60
#define STORAGE_PAGES               4
#define STORAGE_PAGE_MAGIC          0x5A65B

typedef struct {
    file_id_t id;
    uint16_t len;
    uint16_t version;
    uint16_t crc;                   // должно быть последним полем для вычисления crc предыдущих полей
    uint8_t __attribute__((aligned(sizeof(flash_wr_t)))) data[];
} file_header_t;

typedef struct {
    uint16_t magic;
    uint16_t erase_cnt;
    file_header_t __attribute__((aligned(sizeof(flash_wr_t)))) file[] ;
} page_header_t;

typedef struct {
    uint16_t used;
    uint16_t usefull;
} storage_page_ctx_t;

static storage_page_ctx_t storage_ctx[STORAGE_PAGES] = {};
static unsigned empty_page_num = 0;

static inline page_header_t * page_header_from_page(unsigned page)
{
    if (page >= STORAGE_PAGES) {
        return 0;
    }
    return (page_header_t *) ((STORAGE_START_PAGE + page) * FLASH_ATOMIC_ERASE_SIZE);
}

static inline storage_erase_page(unsigned page)
{
    flash_erase_page(STORAGE_START_PAGE + page);
}

static inline unsigned file_data_full_size(unsigned size)
{
    return round_up_deg2(size, sizeof(flash_wr_t));
}

static inline unsigned file_record_full_size(unsigned size)
{
    return sizeof(file_header_t) + file_data_full_size(size);
}

static unsigned file_is_empty(const file_header_t * file)
{
    if (file->id != -1) { return 0; };
    // if (file->len != -1) { return 0; };
    // if (file->version != -1) { return 0; };
    // if (file->crc != -1) { return 0; };
    return 1;
}

static file_header_t * next_file(const file_header_t * file)
{
    file_header_t * next_file = (file_header_t *) ((uint8_t *) file + file_record_full_size(file->len));
    if (file_is_empty(next_file)) {
        return 0;
    }
    return next_file;
}

static file_header_t * file_by_offset_in_page(unsigned page, unsigned offset)
{
    if (offset >= (FLASH_ATOMIC_ERASE_SIZE - sizeof(file_header_t))) {
        return 0;
    }
    return (file_header_t *) ((uint8_t *) page_header_from_page(page) + offset);
}

static uint16_t calc_crc(const void * data, unsigned len, uint16_t iv)
{
    uint16_t crc = iv;
    uint8_t * p = data;
    while (len--) {
        crc += *p++;
    }
    return crc;
}

unsigned page_for_write(unsigned full_file_size)
{
    // здесь нужно выбрать страницу для записи
    // это должна быть страница в которой есть место для записи файла, и максимальное количество уже записанных данных
    // страница с минимальным счетчиком стирания
}


static inline file_header_t * page_max_file(page_header_t * page_header)
{
    return file_by_offset_in_page(page_header, FLASH_ATOMIC_ERASE_SIZE - sizeof(file_header_t));
}

static inline unsigned is_file_version_older(file_header_t * old, file_header_t * test_file)
{
    int diff = old->version - test_file->version;
    if (diff < 0) {
        return 1;
    }
    return 0;
}

static unsigned search_file_in_page(unsigned page, file_id_t id, file_header_t ** result_file)
{
    page_header_t * ph = page_header_from_page(page);
    file_header_t * file = ph->file;
    file_header_t * max_file = page_max_file(ph);
    while (file) {
        if (file->id == id) {
            if ((result_file == 0) ||
                (is_file_version_newer(*result_file, file))
            ) {
                *result_file = file;
            }
        }
        file = next_file(file);
        if (file >= max_file) {
            break;
        }
    }
    return 0;
}

static file_header_t * search_file_by_id(file_id_t id, unsigned * page)
{
    file_header_t * file = 0;
    for (unsigned i = 0; i < STORAGE_PAGES; i++) {
        if (search_file_in_page(i, id, &file)) {
            *page = i;
        }
    }
    return file;
}

static void move_usefull_files_from_page(unsigned page)
{
    // нужно перенести все полезные файлы из страницы page в другую страницу
    page_header_t * ph = page_header_from_page(page);
    file_header_t * file = ph->file;
    file_header_t * max_file = page_max_file(ph);
    while (file) {
        if (file->len) {
            file_id_t id = file->id;
            file_header_t * last_version_of_file = search_file_by_id(id);
            if (file == last_version_of_file) {
                save_file(file->id, file->data, file->len, file, page);
            }
        }

        file = next_file(file);
        if (file >= max_file) {
            break;
        }
    }
}

static void storage_clean_page(unsigned page)
{
    page_header_t * ph = page_header_from_page(page);
    page_header_t header;
    header.magic = STORAGE_PAGE_MAGIC;
    header.erase_cnt = ph->erase_cnt + 1;
    storage_ctx[page].used = FLASH_ATOMIC_ERASE_SIZE;
    move_usefull_files_from_page(page);
    storage_erase_page(page);
    flash_write(ph, &header, sizeof(header));
    storage_ctx[page].usefull = 0;
    storage_ctx[page].used = sizeof(page_header_t);
}

void storage_prepare_page(void)
{
    // нужно добиться того чтобы количество пустых страниц было не 0
    // стирать нужно страницу с минимальным количеством полезных данных
    if (empty_page_num) {
        return;
    }

    unsigned page_for_erase = 0;
    for (unsigned i = 0; i < STORAGE_PAGES; i++) {
        // решаем какую страницу стирать
        
    }

    storage_clean_page(page_for_erase);
    empty_page_num++;
}

static void page_update_ctx_after_write(unsigned new_page, unsigned full_file_size)
{
    if (storage_ctx[new_page].used == sizeof(page_header_t)) {
        empty_page_num--;
    }
    storage_ctx[new_page].used += full_file_size;
    storage_ctx[new_page].usefull += full_file_size;
}

static void * save_file(file_id_t id, const void * data, unsigned len, const file_header_t * old_file, unsigned old_page)
{
    unsigned full_file_size = file_record_full_size(len);
    unsigned new_page = page_for_write(full_file_size);
    file_header_t * new_file = file_by_offset_in_page(new_page, storage_ctx[new_page].used);

    file_header_t header;
    header.id = id;
    header.len = len;

    if (old_file) {
        header.version = old_file->version + 1;
    } else {
        header.version = 0;
    }

    uint16_t crc = calc_crc(data, len, 0);
    header.crc = calc_crc(&header, offsetof(file_header_t, crc), crc);

    flash_write(new_file->data, data, len);
    flash_write(new_file, &header, sizeof(header));

    if (old_file) {
        storage_ctx[old_page].usefull -= file_data_full_size(old_file->len);
    }

    page_update_ctx_after_write(new_page, full_file_size);
    return new_file;
}

void * storage_search_file(file_id_t id, unsigned * len)
{
    unsigned page = 0;
    file_header_t * file = search_file_by_id(id, &page);
    if (file) {
        *len = file->len;
        return file->data;
    }
    return 0;
}

void * storage_write_file(file_id_t id, void * data, unsigned len)
{
    unsigned page = 0;
    file_header_t * file = search_file_by_id(id, &page);
    // save file проигнорирует page если file == 0
    file_header_t * new_file = save_file(id, data, len, file, page);
    return new_file->data;
}

void storage_init(void)
{
    // нужно посчитать использованное место и полезные данные - так как полезное место это метрика для выбора страницы для стирания
    // и нужно посчитать количество пустых страниц
}
