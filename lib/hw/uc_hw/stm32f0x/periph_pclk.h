#include "pclk.h"

#define PCLK_DMA         (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_DMAEN }           /*!< DMA clock enable */
#define PCLK_DMA2        (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_DMA2EN }          /*!< DMA2 clock enable */
#define PCLK_SRAM        (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_SRAMEN }          /*!< SRAM interface clock enable */
#define PCLK_FLITF       (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_FLITFEN }         /*!< FLITF clock enable */
#define PCLK_CRC         (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_CRCEN }           /*!< CRC clock enable */
#define PCLK_GPIOA       (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_GPIOAEN }         /*!< GPIOA clock enable */
#define PCLK_GPIOB       (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_GPIOBEN }         /*!< GPIOB clock enable */
#define PCLK_GPIOC       (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_GPIOCEN }         /*!< GPIOC clock enable */
#define PCLK_GPIOD       (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_GPIODEN }         /*!< GPIOD clock enable */
#define PCLK_GPIOE       (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_GPIOEEN }         /*!< GPIOE clock enable */
#define PCLK_GPIOF       (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_GPIOFEN }         /*!< GPIOF clock enable */
#define PCLK_TSC         (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_TSCEN }           /*!< TS controller clock enable */

#define PCLK_SYSCFGCOMP  (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_SYSCFGCOMPEN }   /*!< SYSCFG and comparator clock enable */
#define PCLK_USART6      (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_USART6EN }       /*!< USART6 clock enable */
#define PCLK_USART7      (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_USART7EN }       /*!< USART7 clock enable */
#define PCLK_USART8      (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_USART8EN }       /*!< USART8 clock enable */
#define PCLK_ADC         (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_ADCEN }          /*!< ADC1 clock enable */
#define PCLK_TIM1        (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_TIM1EN }         /*!< TIM1 clock enable */
#define PCLK_SPI1        (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_SPI1EN }         /*!< SPI1 clock enable */
#define PCLK_USART1      (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_USART1EN }       /*!< USART1 clock enable */
#define PCLK_TIM15       (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_TIM15EN }        /*!< TIM15 clock enable */
#define PCLK_TIM16       (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_TIM16EN }        /*!< TIM16 clock enable */
#define PCLK_TIM17       (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_TIM17EN }        /*!< TIM17 clock enable */
#define PCLK_DBGMCU      (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_DBGMCUEN }       /*!< DBGMCU clock enable */

#define PCLK_TIM2        (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_TIM2EN }         /*!< Timer 2 clock enable */
#define PCLK_TIM3        (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_TIM3EN }         /*!< Timer 3 clock enable */
#define PCLK_TIM6        (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_TIM6EN }         /*!< Timer 6 clock enable */
#define PCLK_TIM7        (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_TIM7EN }         /*!< Timer 7 clock enable */
#define PCLK_TIM14       (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_TIM14EN }        /*!< Timer 14 clock enable */
#define PCLK_WWDG        (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_WWDGEN }         /*!< Window Watchdog clock enable */
#define PCLK_SPI2        (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_SPI2EN }         /*!< SPI2 clock enable */
#define PCLK_USART2      (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_USART2EN }       /*!< USART2 clock enable */
#define PCLK_USART3      (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_USART3EN }       /*!< USART3 clock enable */
#define PCLK_USART4      (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_USART4EN }       /*!< USART4 clock enable */
#define PCLK_USART5      (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_USART5EN }       /*!< USART5 clock enable */
#define PCLK_I2C1        (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_I2C1EN }         /*!< I2C1 clock enable */
#define PCLK_I2C2        (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_I2C2EN }         /*!< I2C2 clock enable */
#define PCLK_USB         (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_USBEN }          /*!< USB clock enable */
#define PCLK_CAN         (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_CANEN }          /*!< CAN clock enable */
#define PCLK_CRS         (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_CRSEN }          /*!< CRS clock enable */
#define PCLK_PWR         (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_PWREN }          /*!< PWR clock enable */
#define PCLK_DAC         (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_DACEN }          /*!< DAC clock enable */
#define PCLK_CEC         (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_CECEN }          /*!< CEC clock enable */
