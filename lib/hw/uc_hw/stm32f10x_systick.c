#include "stm32f10x.h"
#include "hw_rcc.h"
#include "time_units.h"
#include "irq_vectors.h"
#include "time_moment.h"

volatile unsigned uptime_ms = 0;
static unsigned systicks_in_ms;
// static unsigned systicks_in_us;

void systick_irq_handler(void)
{
    uptime_ms++;
}

void init_systick(void)
{
    NVIC_SetHandler(SysTick_IRQn, &systick_irq_handler);
    NVIC_EnableIRQ(SysTick_IRQn);
    systicks_in_ms = hw_rcc_f_hclk() / MS_IN_S;
    // systicks_in_us = systicks_in_ms / US_IN_MS;
    SysTick->LOAD = systicks_in_ms - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE + SysTick_CTRL_TICKINT + SysTick_CTRL_ENABLE;
}

void time_moment_save(time_moment_t * tm)
{
    tm->ms = uptime_ms;
    tm->systick = SysTick->VAL;
    if (tm->ms != uptime_ms) {
        tm->ms = uptime_ms;
        tm->systick = SysTick->VAL;
    }
}

unsigned time_moment_interval_cycles(time_moment_t * start, time_moment_t * end)
{
    unsigned ticks = start->systick - end->systick;
    unsigned ms = end->ms - start->ms;
    ticks += ms * systicks_in_ms;
    return ticks;
}
