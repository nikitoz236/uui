#include <stdint.h>

typedef uint16_t file_id_t;

void * storage_search_file(file_id_t id, unsigned * len);
void * storage_write_file(file_id_t id, void * data, unsigned len);
void storage_init(void);
void storage_prepare_page(void);
