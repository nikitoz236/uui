#include "pclk.h"

#define PCLK_DMA1       (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_DMA1EN }                 /*!< DMA1 clock enable */
#define PCLK_SRAM       (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_SRAMEN }                 /*!< SRAM interface clock enable */
#define PCLK_FLITF      (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_FLITFEN }                /*!< FLITF clock enable */
#define PCLK_CRC        (pclk_t){ .bus = PCLK_BUS_AHB,  .mask = RCC_AHBENR_CRCEN }                  /*!< CRC clock enable */

#define PCLK_AFIO       (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_AFIOEN }                /*!< Alternate Function I/O clock enable */
#define PCLK_IOPA       (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_IOPAEN }                /*!< I/O port A clock enable */
#define PCLK_IOPB       (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_IOPBEN }                /*!< I/O port B clock enable */
#define PCLK_IOPC       (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_IOPCEN }                /*!< I/O port C clock enable */
#define PCLK_IOPD       (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_IOPDEN }                /*!< I/O port D clock enable */
#define PCLK_IOPE       (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_IOPEEN }                /*!< I/O port E clock enable */
#define PCLK_ADC1       (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_ADC1EN }                /*!< ADC 1 interface clock enable */
#define PCLK_ADC2       (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_ADC2EN }                /*!< ADC 2 interface clock enable */
#define PCLK_TIM1       (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_TIM1EN }                /*!< TIM1 Timer clock enable */
#define PCLK_SPI1       (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_SPI1EN }                /*!< SPI 1 clock enable */
#define PCLK_USART1     (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_USART1EN }              /*!< USART1 clock enable */
#define PCLK_TIM15      (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_TIM15EN }               /*!< TIM15 Timer clock enable */
#define PCLK_TIM16      (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_TIM16EN }               /*!< TIM16 Timer clock enable */
#define PCLK_TIM17      (pclk_t){ .bus = PCLK_BUS_APB2, .mask = RCC_APB2ENR_TIM17EN }               /*!< TIM17 Timer clock enable */

#define PCLK_TIM2       (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_TIM2EN }                /*!< Timer 2 clock enabled*/
#define PCLK_TIM3       (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_TIM3EN }                /*!< Timer 3 clock enable */
#define PCLK_WWDG       (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_WWDGEN }                /*!< Window Watchdog clock enable */
#define PCLK_USART2     (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_USART2EN }              /*!< USART 2 clock enable */
#define PCLK_I2C1       (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_I2C1EN }                /*!< I2C 1 clock enable */
#define PCLK_CAN1       (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_CAN1EN }                /*!< CAN1 clock enable */
#define PCLK_BKP        (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_BKPEN }                 /*!< Backup interface clock enable */
#define PCLK_PWR        (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_PWREN }                 /*!< Power interface clock enable */
#define PCLK_TIM4       (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_TIM4EN }                /*!< Timer 4 clock enable */
#define PCLK_SPI2       (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_SPI2EN }                /*!< SPI 2 clock enable */
#define PCLK_USART3     (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_USART3EN }              /*!< USART 3 clock enable */
#define PCLK_I2C2       (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_I2C2EN }                /*!< I2C 2 clock enable */
#define PCLK_USB        (pclk_t){ .bus = PCLK_BUS_APB1, .mask = RCC_APB1ENR_USBEN }                 /*!< USB Device clock enable */
