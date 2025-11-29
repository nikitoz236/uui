#include <stdint.h>

#include "soc/timer_group_struct.h"
#include "soc/rtc_cntl_struct.h"

int main(void);

void disable_wdt(void)
{
    // Отключаем системный WDT TG0
    TIMERG0.wdtwprotect.val = 0x50D83AA1;       // Разблокировка
    TIMERG0.wdtfeed.val = 1;                    // Сброс счётчика
    TIMERG0.wdtconfig0.val = 0;                 // Отключаем WDT
    TIMERG0.wdtwprotect.val = 0;                // Блокируем

    // Отключаем системный WDT RTC
    RTCCNTL.wdt_wprotect = 0x50D83AA1;          // Разблокировка
    RTCCNTL.wdt_config0.val = 0;                // Отключаем WDT
    RTCCNTL.ext_xtl_conf.xtal32k_wdt_en = 0;    // Отключаем внешний кристалл
    RTCCNTL.wdt_wprotect = 0;                   // Блокируем

    RTCCNTL.swd_conf.swd_disable = 1;           // Отключаем Super WDT
}

void _reset(void)
{
    extern uint32_t _sbss;
    extern uint32_t _ebss;
    uint32_t * bss = &_sbss;
    while (bss < &_ebss) {
        *bss++ = 0;
    }

    disable_wdt();

    main();
}
