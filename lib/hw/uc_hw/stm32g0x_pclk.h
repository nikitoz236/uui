#include "pclk.h"

#define PCLK_GPIOA          (pclk_t){ .bus = PCLK_BUS_IOP,  .mask = (0x1UL << 0U) }
#define PCLK_GPIOB          (pclk_t){ .bus = PCLK_BUS_IOP,  .mask = (0x1UL << 1U) }
#define PCLK_GPIOC          (pclk_t){ .bus = PCLK_BUS_IOP,  .mask = (0x1UL << 2U) }
#define PCLK_GPIOD          (pclk_t){ .bus = PCLK_BUS_IOP,  .mask = (0x1UL << 3U) }
#define PCLK_GPIOF          (pclk_t){ .bus = PCLK_BUS_IOP,  .mask = (0x1UL << 5U) }

#define PCLK_DMA1           (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = (0x1UL << 0U) }
#define PCLK_FLASH          (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = (0x1UL << 8U) }
#define PCLK_CRC            (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = (0x1UL << 12U) }

#define PCLK_TIM3           (pclk_t){ .bus = PCLK_BUS_APB1, .mask = (0x1UL << 1U) }
#define PCLK_RTCAPB         (pclk_t){ .bus = PCLK_BUS_APB1, .mask = (0x1UL << 10U) }
#define PCLK_WWDG           (pclk_t){ .bus = PCLK_BUS_APB1, .mask = (0x1UL << 11U) }
#define PCLK_SPI2           (pclk_t){ .bus = PCLK_BUS_APB1, .mask = (0x1UL << 14U) }
#define PCLK_USART2         (pclk_t){ .bus = PCLK_BUS_APB1, .mask = (0x1UL << 17U) }
#define PCLK_I2C1           (pclk_t){ .bus = PCLK_BUS_APB1, .mask = (0x1UL << 21U) }
#define PCLK_I2C2           (pclk_t){ .bus = PCLK_BUS_APB1, .mask = (0x1UL << 22U) }
#define PCLK_DBG            (pclk_t){ .bus = PCLK_BUS_APB1, .mask = (0x1UL << 27U) }
#define PCLK_PWR            (pclk_t){ .bus = PCLK_BUS_APB1, .mask = (0x1UL << 28U) }

#define PCLK_SYSCFG         (pclk_t){ .bus = PCLK_BUS_APB2, .mask = (0x1UL << 0U) }
#define PCLK_TIM1           (pclk_t){ .bus = PCLK_BUS_APB2, .mask = (0x1UL << 11U) }
#define PCLK_SPI1           (pclk_t){ .bus = PCLK_BUS_APB2, .mask = (0x1UL << 12U) }
#define PCLK_USART1         (pclk_t){ .bus = PCLK_BUS_APB2, .mask = (0x1UL << 14U) }
#define PCLK_TIM14          (pclk_t){ .bus = PCLK_BUS_APB2, .mask = (0x1UL << 15U) }
#define PCLK_TIM16          (pclk_t){ .bus = PCLK_BUS_APB2, .mask = (0x1UL << 17U) }
#define PCLK_TIM17          (pclk_t){ .bus = PCLK_BUS_APB2, .mask = (0x1UL << 18U) }
#define PCLK_ADC            (pclk_t){ .bus = PCLK_BUS_APB2, .mask = (0x1UL << 20U) }
