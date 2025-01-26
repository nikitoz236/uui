#include "pclk.h"

#define PCLK_DMA         { .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_DMAEN }           /*!< DMA clock enable */
#define PCLK_DMA2        { .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_DMA2EN }          /*!< DMA2 clock enable */
#define PCLK_SRAM        { .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_SRAMEN }          /*!< SRAM interface clock enable */
#define PCLK_FLITF       { .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_FLITFEN }         /*!< FLITF clock enable */
#define PCLK_CRC         { .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_CRCEN }           /*!< CRC clock enable */
#define PCLK_GPIOA       { .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_GPIOAEN }         /*!< GPIOA clock enable */
#define PCLK_GPIOB       { .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_GPIOBEN }         /*!< GPIOB clock enable */
#define PCLK_GPIOC       { .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_GPIOCEN }         /*!< GPIOC clock enable */
#define PCLK_GPIOD       { .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_GPIODEN }         /*!< GPIOD clock enable */
#define PCLK_GPIOE       { .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_GPIOEEN }         /*!< GPIOE clock enable */
#define PCLK_GPIOF       { .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_GPIOFEN }         /*!< GPIOF clock enable */
#define PCLK_TSC         { .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_TSCEN }           /*!< TS controller clock enable */

#define PCLK_SYSCFGCOMP  { .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_SYSCFGCOMPEN }   /*!< SYSCFG and comparator clock enable */
#define PCLK_USART6      { .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_USART6EN }       /*!< USART6 clock enable */
#define PCLK_USART7      { .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_USART7EN }       /*!< USART7 clock enable */
#define PCLK_USART8      { .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_USART8EN }       /*!< USART8 clock enable */
#define PCLK_ADC         { .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_ADCEN }          /*!< ADC1 clock enable */
#define PCLK_TIM1        { .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_TIM1EN }         /*!< TIM1 clock enable */
#define PCLK_SPI1        { .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_SPI1EN }         /*!< SPI1 clock enable */
#define PCLK_USART1      { .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_USART1EN }       /*!< USART1 clock enable */
#define PCLK_TIM15       { .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_TIM15EN }        /*!< TIM15 clock enable */
#define PCLK_TIM16       { .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_TIM16EN }        /*!< TIM16 clock enable */
#define PCLK_TIM17       { .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_TIM17EN }        /*!< TIM17 clock enable */
#define PCLK_DBGMCU      { .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_DBGMCUEN }       /*!< DBGMCU clock enable */

#define PCLK_TIM2        { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_TIM2EN }         /*!< Timer 2 clock enable */
#define PCLK_TIM3        { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_TIM3EN }         /*!< Timer 3 clock enable */
#define PCLK_TIM6        { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_TIM6EN }         /*!< Timer 6 clock enable */
#define PCLK_TIM7        { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_TIM7EN }         /*!< Timer 7 clock enable */
#define PCLK_TIM14       { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_TIM14EN }        /*!< Timer 14 clock enable */
#define PCLK_WWDG        { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_WWDGEN }         /*!< Window Watchdog clock enable */
#define PCLK_SPI2        { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_SPI2EN }         /*!< SPI2 clock enable */
#define PCLK_USART2      { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_USART2EN }       /*!< USART2 clock enable */
#define PCLK_USART3      { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_USART3EN }       /*!< USART3 clock enable */
#define PCLK_USART4      { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_USART4EN }       /*!< USART4 clock enable */
#define PCLK_USART5      { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_USART5EN }       /*!< USART5 clock enable */
#define PCLK_I2C1        { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_I2C1EN }         /*!< I2C1 clock enable */
#define PCLK_I2C2        { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_I2C2EN }         /*!< I2C2 clock enable */
#define PCLK_USB         { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_USBEN }          /*!< USB clock enable */
#define PCLK_CAN         { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_CANEN }          /*!< CAN clock enable */
#define PCLK_CRS         { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_CRSEN }          /*!< CRS clock enable */
#define PCLK_PWR         { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_PWREN }          /*!< PWR clock enable */
#define PCLK_DAC         { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_DACEN }          /*!< DAC clock enable */
#define PCLK_CEC         { .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_CECEN }          /*!< CEC clock enable */
