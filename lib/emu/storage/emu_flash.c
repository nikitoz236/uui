#include "flash_hw.h"
#include "storage_hw.h"

static uint8_t flash_mem[STORAGE_PAGES][FLASH_ATOMIC_ERASE_SIZE];
static unsigned flash_size;

void * storage_page_to_pointer(unsigned page)
{
    return flash_mem[page];
}

void storage_erase_page(unsigned page)
{
    if (page >= STORAGE_PAGES) {
        return;
    }
    for (unsigned i = 0; i < FLASH_ATOMIC_ERASE_SIZE; i++) {
        flash_mem[page][i] = 0xFF;
    }
}

void flash_write(const void * addr, const void * data, unsigned size)
{
    flash_wr_t * dst = (flash_wr_t *)addr;
    const flash_wr_t * src = data;
    while (size) {
        *dst++ = *src++;
        if (size >= sizeof(flash_wr_t)) {
            size -= sizeof(flash_wr_t);
        } else {
            size = 0;
        }
    }
}