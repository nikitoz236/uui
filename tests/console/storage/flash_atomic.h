#pragma once
#include <stdint.h>

// файл определяет атомарный размер слова для записи через тип flash_wr_t
// и размер страницы для атомарного стирания FLASH_ATOMIC_ERASE_SIZE

typedef uint16_t flash_wr_t;

#define FLASH_ATOMIC_ERASE_SIZE     1024
