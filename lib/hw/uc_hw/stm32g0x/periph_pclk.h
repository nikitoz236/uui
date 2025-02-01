#include "pclk.h"

#define PCLK_GPIOA          { .bus = PCLK_BUS_IOP,  .mask = (0x1UL << 0U) }
#define PCLK_GPIOB          { .bus = PCLK_BUS_IOP,  .mask = (0x1UL << 1U) }
#define PCLK_GPIOC          { .bus = PCLK_BUS_IOP,  .mask = (0x1UL << 2U) }
#define PCLK_GPIOD          { .bus = PCLK_BUS_IOP,  .mask = (0x1UL << 3U) }
#define PCLK_GPIOF          { .bus = PCLK_BUS_IOP,  .mask = (0x1UL << 5U) }

#define PCLK_DMA1           { .bus = PCLK_BUS_AHB,  .mask = (0x1UL << 0U) }
#define PCLK_FLASH          { .bus = PCLK_BUS_AHB,  .mask = (0x1UL << 8U) }
#define PCLK_CRC            { .bus = PCLK_BUS_AHB,  .mask = (0x1UL << 12U) }

#define PCLK_TIM3           { .bus = PCLK_BUS_APB1, .mask = (0x1UL << 1U) }
#define PCLK_RTCAPB         { .bus = PCLK_BUS_APB1, .mask = (0x1UL << 10U) }
#define PCLK_WWDG           { .bus = PCLK_BUS_APB1, .mask = (0x1UL << 11U) }
#define PCLK_SPI2           { .bus = PCLK_BUS_APB1, .mask = (0x1UL << 14U) }
#define PCLK_USART2         { .bus = PCLK_BUS_APB1, .mask = (0x1UL << 17U) }
#define PCLK_I2C1           { .bus = PCLK_BUS_APB1, .mask = (0x1UL << 21U) }
#define PCLK_I2C2           { .bus = PCLK_BUS_APB1, .mask = (0x1UL << 22U) }
#define PCLK_DBG            { .bus = PCLK_BUS_APB1, .mask = (0x1UL << 27U) }
#define PCLK_PWR            { .bus = PCLK_BUS_APB1, .mask = (0x1UL << 28U) }

#define PCLK_SYSCFG         { .bus = PCLK_BUS_APB2, .mask = (0x1UL << 0U) }
#define PCLK_TIM1           { .bus = PCLK_BUS_APB2, .mask = (0x1UL << 11U) }
#define PCLK_SPI1           { .bus = PCLK_BUS_APB2, .mask = (0x1UL << 12U) }
#define PCLK_USART1         { .bus = PCLK_BUS_APB2, .mask = (0x1UL << 14U) }
#define PCLK_TIM14          { .bus = PCLK_BUS_APB2, .mask = (0x1UL << 15U) }
#define PCLK_TIM16          { .bus = PCLK_BUS_APB2, .mask = (0x1UL << 17U) }
#define PCLK_TIM17          { .bus = PCLK_BUS_APB2, .mask = (0x1UL << 18U) }
#define PCLK_ADC            { .bus = PCLK_BUS_APB2, .mask = (0x1UL << 20U) }
