#include "flash_atomic.h"

#define FLASH_ERASED                ((flash_wr_t)(-1))

void flash_erase_page(unsigned page);
void flash_write(const void * addr, const void * data, unsigned size);
