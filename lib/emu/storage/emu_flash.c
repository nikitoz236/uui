#include "flash_hw.h"

static uint8_t * flash_start;
static unsigned flash_size;

void init_flash_memory(void * start, unsigned size)
{
    flash_start = start;
    flash_size = size;
}

void flash_erase_page(unsigned page)
{
    if (page >= (flash_size / FLASH_ATOMIC_ERASE_SIZE)) {
        return;
    }
    uint8_t * ptr = flash_start[page * FLASH_ATOMIC_ERASE_SIZE];
    for (unsigned i = 0; i < FLASH_ATOMIC_ERASE_SIZE; i++) {
        ptr[i] = 0xFF;
    }
}

void flash_write(void * addr, const void * data, unsigned size)
{
    uint8_t * ptr = addr;
    for (unsigned i = 0; i < size; i++) {
        ptr[i] = ((uint8_t *)data)[i];
    }
}
