#include <stdio.h>
#include "storage_hw.h"
#include "flash_atomic.h"

static uint8_t flash_mem[STORAGE_PAGES][FLASH_ATOMIC_ERASE_SIZE];

const char storag_file_image[] = "./storage.img";

void emu_storage_load(void)
{
    FILE * f = fopen(storag_file_image, "rb");
    if (f) {
        fread(flash_mem, sizeof(flash_mem), 1, f);
        fclose(f);
    }
}

static void emu_storage_write_file_image(void)
{
    FILE * f = fopen(storag_file_image, "wb");
    if (f) {
        fwrite(flash_mem, sizeof(flash_mem), 1, f);
        fclose(f);
    }
}

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
    emu_storage_write_file_image();
}

void storage_write(const void * addr, const void * data, unsigned size)
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
    emu_storage_write_file_image();
}
