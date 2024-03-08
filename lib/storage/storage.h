#include <stdint.h>

typedef uint16_t file_id_t;

const void * storage_search_file(file_id_t id, unsigned * len);
const void * storage_write_file(file_id_t id, const void * data, unsigned len);
void storage_init(void);
void storage_prepare_page(void);
void storage_print_info(void);
