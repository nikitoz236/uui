#pragma once
#include <stdint.h>

typedef uint16_t flash_wr_t;

#define FLASH_ATOMIC_ERASE_SIZE     1024

#define STORAGE_PAGES               4

const void * storage_page_to_pointer(unsigned page);
void storage_erase_page(unsigned page);
void storage_write(const void * addr, const void * data, unsigned size);
