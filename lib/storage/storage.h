#include <stdint.h>

/*
    модуль реализует файловую систему внутри flash микроконтроллера

    особенность в том что данные всегда доступны по указателю

    для работу модулю необходим следующее окружение:
        storage_hw.h    - прослойка для доступа к 3м функциям
            flash_atomic.h
                зависит от микроконтроллера, содержит:
                    тип flash_wr_t - минимальная атомарная запись
                    FLASH_ATOMIC_ERASE_SIZE - размер страницы для стирания

            static inline const void * storage_page_to_pointer(unsigned page);
                преобразует номер страницы в указатель на начало страницы

            void storage_erase_page(unsigned page);
                стирание страницы по номеру (не всегда в микроконтрллере можно стереть страницу по указателю)

            void storage_write(const void * addr, const void * data, unsigned size);
                запись данных во flash

            также во storage_hw.h определяется количество страниц под файловую систему - STORAGE_PAGES

    планируется что в проекте функции записи и стирания подменяются на соответствующие для работы с flash
    расположение файловой системы определяется через функцию storage_page_to_pointer

    вот думаю что flash_atomic можно инклудить в storage_hw.h, так как он в любом случае специфичен для проекта
*/

typedef uint16_t file_id_t;

const void * storage_search_file(file_id_t id, unsigned * len);
const void * storage_write_file(file_id_t id, const void * data, unsigned len);
void storage_init(void);
void storage_prepare_page(void);
void storage_print_info(void);
