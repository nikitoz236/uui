#include "storage.h"
#include "misc.h"
#include "stddef.h"
#include "storage_hw.h"
#include "flash_atomic.h"

#define STORAGE_PAGE_MAGIC          0x5A6B
#define MAX_FILE_OFFSET_IN_PAGE     (FLASH_ATOMIC_ERASE_SIZE - sizeof(file_header_t))
#define FILE_VERSION_START          65530
#define PAGE_ERASE_CNT_START        65530

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

static inline const  page_header_t * page_header_from_page(unsigned page)
{
    return (page_header_t *) storage_page_to_pointer(page);
}

static inline unsigned file_data_full_size(unsigned size)
{
    return round_up_deg2(size, sizeof(flash_wr_t));
}

static inline unsigned file_record_full_size(unsigned size)
{
    return sizeof(file_header_t) + file_data_full_size(size);
}

static unsigned flash_is_erased(const void * ptr, unsigned len)
{
    unsigned result = 1;
    const uint8_t * p = ptr;
    while (len--) {
        if (*p++ != 0xFF) {
            return 0;
        }
    }
    return 1;
}

static unsigned file_is_valid(const file_header_t * file)
{
    if (file->id == (file_id_t)-1) {
        return 0;
    }
    // if (file->len != -1) { return 0; };
    // if (file->version != -1) { return 0; };
    // if (file->crc != -1) { return 0; };
    return 1;
}

static const file_header_t * next_file(const file_header_t * file)
{
    const file_header_t * next_file = (file_header_t *) ((uint8_t *) file + file_record_full_size(file->len));
    if (file_is_valid(next_file)) {
        return next_file;
    }
    return 0;
}

static const file_header_t * file_by_offset_in_page(const page_header_t * page_header, unsigned offset)
{
    if (offset > MAX_FILE_OFFSET_IN_PAGE) {
        return 0;
    }
    return (file_header_t *) ((uint8_t *) page_header + offset);
}

static uint16_t calc_crc(const void * data, unsigned len, uint16_t iv)
{
    uint16_t crc = iv;
    const uint8_t * p = data;
    while (len--) {
        crc += *p++;
    }
    return crc;
}

unsigned page_for_write(unsigned full_file_record_size)
{
    // здесь нужно выбрать страницу для записи
    // это должна быть страница в которой есть место для записи файла, и максимальное количество уже записанных данных
    // страница с минимальным счетчиком стирания
    unsigned page = 0;
    unsigned max_acceptable = FLASH_ATOMIC_ERASE_SIZE - full_file_record_size;
    unsigned max_used = 0;


    for (unsigned i = 0; i < STORAGE_PAGES; i++) {
        if (storage_ctx[i].used <= max_acceptable) {
            if (storage_ctx[i].used > max_used) {
                max_used = storage_ctx[i].used;
                page = i;
            }
        }
    }

    printf("    search_page_for_write for size %d, max_acceptable %d - page %d\n", full_file_record_size, max_acceptable, page);

    return page;
}

// максимальный возможный указатель на файл в странице
static inline const file_header_t * page_max_file(const page_header_t * page_header)
{
    return file_by_offset_in_page(page_header, MAX_FILE_OFFSET_IN_PAGE);
}

static inline unsigned is_file_version_newer(const file_header_t * old, const file_header_t * file)
{
    int16_t diff = (int16_t)file->version - (int16_t)old->version;
    if (diff > 0) {
        return 1;
    }
    return 0;
}

static const file_header_t * first_file_in_page_header(const page_header_t * ph)
{
    const file_header_t * file = ph->file;
    if (file_is_valid(file)) {
        return file;
    }
    return 0;
}

static unsigned file_is_crc_correct(const file_header_t * file)
{
    uint16_t crc = calc_crc(file->data, file->len, 0);
    crc = calc_crc(file, offsetof(file_header_t, crc), crc);
    if (crc == file->crc) {
        return 1;
    }
    return 0;
}

// макрос обеспечивает цикл по всем файлам на странице, не выходя за пределы страницы
#define for_each_file_in_page_header(ph) \
    const file_header_t * max_file = page_max_file(ph); \
    for (const file_header_t * file = first_file_in_page_header(ph); \
        file && (file <= max_file); \
        file = next_file(file))

static unsigned search_file_in_page(unsigned page, file_id_t id, const file_header_t ** result_file)
{
    unsigned ret = 0;
    const page_header_t * ph = page_header_from_page(page);

    for_each_file_in_page_header(ph) {
        if (file->id == id) {
            printf("    search_file_in_page finded file %p id %d ver %d in page %d\n", file, file->id, file->version, page);
            if (file_is_crc_correct(file)) {
                if ((*result_file == 0) ||
                    (is_file_version_newer(*result_file, file))
                ) {
                    *result_file = file;
                    ret = 1;
                }
            }
        }
    }
    return ret;
}

static const file_header_t * search_file_by_id(file_id_t id, unsigned * page)
{
    const file_header_t * file = 0;
    for (unsigned i = 0; i < STORAGE_PAGES; i++) {
        if (search_file_in_page(i, id, &file)) {
            *page = i;
        }
    }
    return file;
}

static void page_update_ctx_after_write(unsigned new_page, unsigned full_file_size)
{
    if (storage_ctx[new_page].used == sizeof(page_header_t)) {
        empty_page_num--;
    }
    storage_ctx[new_page].used += full_file_size;
    storage_ctx[new_page].usefull += full_file_size;

}

static const void * save_file(file_id_t id, const void * data, unsigned len, const file_header_t * old_file, unsigned old_page)
{
    unsigned full_file_size = file_record_full_size(len);
    unsigned new_page = page_for_write(full_file_size);
    const page_header_t * ph = page_header_from_page(new_page);
    const file_header_t * new_file = file_by_offset_in_page(ph, storage_ctx[new_page].used);

    file_header_t header;
    header.id = id;
    header.len = len;

    if (old_file) {
        header.version = old_file->version + 1;
        // printf("                    save_file old file %p id %d ver %d new ver %d\n", old_file, old_file->id, old_file->version, header.version);
    } else {
        header.version = FILE_VERSION_START;
    }

    uint16_t crc = calc_crc(data, len, 0);
    header.crc = calc_crc(&header, offsetof(file_header_t, crc), crc);

    storage_write(new_file->data, data, len);
    storage_write(new_file, &header, sizeof(header));

    if (old_file) {
        storage_ctx[old_page].usefull -= file_record_full_size(old_file->len);
    }

    page_update_ctx_after_write(new_page, full_file_size);
    return new_file;
}

static void move_usefull_files_from_page(unsigned page)
{
    // нужно перенести все полезные файлы из страницы page в другую страницу
    printf("    move_usefull_files_from_page page %d\n", page);

    // чтоб данная страница не выбиралась для записи
    storage_ctx[page].used = FLASH_ATOMIC_ERASE_SIZE;

    const page_header_t * ph = page_header_from_page(page);
    const file_header_t * last_version_of_file = 0;

    for_each_file_in_page_header(ph) {
        if (file->len) {
            printf("            move_usefull_files_from_page file %p id %d ver %d\n", file, file->id, file->version);
            // может быть такое что файл перезаписывался несколько раз подряд
            // в этом случае не нужно искать последнюю версию файла повторно
            if ((last_version_of_file == 0) || (last_version_of_file->id != file->id)) {
                unsigned file_page;
                last_version_of_file = search_file_by_id(file->id, &file_page);
            }
            if (file == last_version_of_file) {
                save_file(file->id, file->data, file->len, file, page);
            }
        }
    }
}

static void storage_page_add_header(const page_header_t * ph, unsigned counter)
{
    page_header_t header;
    header.magic = STORAGE_PAGE_MAGIC;
    header.erase_cnt = counter;
    storage_write(ph, &header, sizeof(header));
}

static void storage_clean_page(unsigned page)
{
    printf("    storage_clean_page page %d\n", page);

    const page_header_t * ph = page_header_from_page(page);
    unsigned cnt = ph->erase_cnt + 1;
    storage_erase_page(page);

    printf("    page erased, write header with counter %d\n", cnt);

    storage_page_add_header(ph, cnt);
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

    printf("\n\n\n  storage_prepare_page\n");
    storage_print_info();

    unsigned page_for_erase = 0;
    unsigned min_metric = -1;
    for (unsigned i = 0; i < STORAGE_PAGES; i++) {
        // решаем какую страницу стирать, нужно ту в которой меньше всего полезных данных, и больше всего использовано
        // если таких страниц несколько то стирать нужно ту у которой меньше счетчик стираний
        unsigned free = FLASH_ATOMIC_ERASE_SIZE - storage_ctx[i].used;
        // на случай если free + usefull = 0 нам нужно будет вычесть откуда то отрицательное значение счетчика стирания
        unsigned metric = free + storage_ctx[i].usefull + 1;
        metric <<= 16;
        // счетчик стирания знаковый для обработки переполнений
        metric += (int16_t)(page_header_from_page(i)->erase_cnt);
        if (min_metric > metric) {
            min_metric = metric;
            page_for_erase = i;
        }
        printf("            storage_prepare_page page %d metric %d\n", i, metric);
    }

    printf("  storage_prepare_page erase %d\n", page_for_erase);

    move_usefull_files_from_page(page_for_erase);
    storage_clean_page(page_for_erase);

    empty_page_num++;

    // storage_print_info();
}


const void * storage_search_file(file_id_t id, unsigned * len)
{
    unsigned page = 0;
    const file_header_t * file = search_file_by_id(id, &page);
    if (file) {
        *len = file->len;
        return file->data;
    }
    return 0;
}

const void * storage_write_file(file_id_t id, const void * data, unsigned len)
{
    if (len > FLASH_ATOMIC_ERASE_SIZE - sizeof(page_header_t) - sizeof(file_header_t)) {
        printf("storage_write_file file %d too big %d\n", id, len);
        return 0;
    }
    unsigned page = 0;
    const file_header_t * file = search_file_by_id(id, &page);
    // save file проигнорирует page если file == 0
    const file_header_t * new_file = save_file(id, data, len, file, page);
    return new_file->data;
}

void storage_init(void)
{
    // нужно посчитать использованное место и полезные данные - так как полезное место это метрика для выбора страницы для стирания
    // и нужно посчитать количество пустых страниц

    for (unsigned i = 0; i < STORAGE_PAGES; i++) {
        const page_header_t * ph = page_header_from_page(i);
        unsigned used = sizeof(page_header_t);
        unsigned usefull = 0;

        if (ph->magic != STORAGE_PAGE_MAGIC) {
            if (!flash_is_erased(ph, FLASH_ATOMIC_ERASE_SIZE)) {
                storage_erase_page(i);
            }

            storage_page_add_header(ph, PAGE_ERASE_CNT_START);
            empty_page_num++;
        } else {
            const file_header_t * last_version_of_file = 0;
            for_each_file_in_page_header(ph) {
                // может быть такое что файл перезаписывался несколько раз подряд
                // в этом случае не нужно искать последнюю версию файла повторно
                if ((last_version_of_file == 0) || (last_version_of_file->id != file->id)) {
                    unsigned file_page;
                    last_version_of_file = search_file_by_id(file->id, &file_page);
                }
                unsigned file_full_size = file_record_full_size(file->len);
                used += file_full_size;
                if (file == last_version_of_file) {
                    usefull += file_full_size;
                }
            }
            for (unsigned idx = used; idx < FLASH_ATOMIC_ERASE_SIZE; idx++) {
                if (((uint8_t*)ph)[idx] != 0xFF) {
                    used = idx;
                }
            }
        }
        storage_ctx[i].usefull = usefull;
        storage_ctx[i].used = used;
    }
}

void storage_print_info(void)
{
    printf("\n+++   stroage info: empty pages %d\n", empty_page_num);
    for (unsigned i = 0; i < STORAGE_PAGES; i++) {
        const page_header_t * ph = page_header_from_page(i);

        printf("  page %2d erased %3d used %5d usefull [%5d] free [%5d]\n", i, ph->erase_cnt, storage_ctx[i].used, storage_ctx[i].usefull, FLASH_ATOMIC_ERASE_SIZE - storage_ctx[i].used);
        for_each_file_in_page_header(ph) {
            printf("                    file id <%05d> ver %5d len %5d full len %5d\n", file->id, file->version, file->len, file_record_full_size(file->len));
        }
    }
}
