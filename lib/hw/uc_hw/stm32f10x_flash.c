#include "stm32f10x_flash.h"
#include "stm32f10x.h"

#define FLASH_KEY1 ((uint32_t)0x45670123)
#define FLASH_KEY2 ((uint32_t)0xCDEF89AB)

static void flash_unlock(void)
{
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
}

static void flash_lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

static inline unsigned flash_is_busy(void)
{
    return (FLASH->SR & FLASH_SR_BSY);
}

const void * flash_page_to_pointer(unsigned page)
{
    return (const void *)(FLASH_BASE + (page * FLASH_ATOMIC_ERASE_SIZE));
}

void flash_erase_page(unsigned page)
{
    flash_unlock();

    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = (uint32_t)flash_page_to_pointer(page);

    FLASH->CR |= FLASH_CR_STRT;
    while(flash_is_busy());
    FLASH->CR &= ~FLASH_CR_PER;

    flash_lock();
}

void flash_write(const void * addr, const void * data, unsigned size)
{
    flash_unlock();
    FLASH->CR |= FLASH_CR_PG;
    while(flash_is_busy());

    volatile flash_wr_t * dst = (flash_wr_t *)addr;
    const flash_wr_t * src = data;

    while (size) {
        *dst++ = *src++;
        if (size >= sizeof(flash_wr_t)) {
            size -= sizeof(flash_wr_t);
        } else {
            size = 0;
        }
    }

    FLASH->CR &= ~FLASH_CR_PG;
    flash_lock();
}
