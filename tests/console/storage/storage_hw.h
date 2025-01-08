#pragma once
#include <stdio.h>

#define STORAGE_PAGES               4

void * storage_page_to_pointer(unsigned page);
void storage_erase_page(unsigned page);
void storage_write(const void * addr, const void * data, unsigned size);
