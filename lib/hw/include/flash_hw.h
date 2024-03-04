#include <stdint.h>

typedef uint16_t flash_wr_t;

#define FLASH_ATOMIC_ERASE_SIZE     1024
#define FLASH_ERASED                ((flash_wr_t)(-1))

void flash_erase_page(unsigned page);
void flash_write(void * addr, const void * data, unsigned size);
