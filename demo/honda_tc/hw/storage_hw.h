#pragma once
#include "stm32f10x_flash.h"

#define STORAGE_START_PAGE          60
#define STORAGE_PAGES               4

static inline const void * storage_page_to_pointer(unsigned page)
{
    return flash_page_to_pointer(STORAGE_START_PAGE + page);
}

static inline void storage_erase_page(unsigned page)
{
    flash_erase_page(STORAGE_START_PAGE + page);
}

static inline void storage_write(const void * addr, const void * data, unsigned size)
{
    flash_write(addr, data, size);
}
