#include "config.h"


// переопределение регистров stm32g0 для совместимости с кодом для stm32f0

// rcc
#define RCC_CFGR_SW_LSI                 (RCC_CFGR_SW_0 | RCC_CFGR_SW_1)
#define RCC_CFGR_SWS_LSI                (RCC_CFGR_SWS_0 | RCC_CFGR_SWS_1)
#define RCC_CFGR_SW_LSE                 (RCC_CFGR_SW_2)
#define RCC_CFGR_SWS_LSE                (RCC_CFGR_SWS_2)
#define RCC_CFGR_SW_PLL                 RCC_CFGR_SW_1
#define RCC_CFGR_SWS_PLL                RCC_CFGR_SWS_1
#define RCC_CFGR_SW_HSE                 RCC_CFGR_SW_0
#define RCC_CFGR_SWS_HSE                RCC_CFGR_SWS_0

// apb
#define APB2ENR                         APBENR2

// syscfg
#define RCC_APB2ENR_SYSCFGEN            RCC_APBENR2_SYSCFGEN

// exti
#define FTSR                            FTSR1
#define RTSR                            RTSR1
#define IMR                             IMR1

// timers
#define RCC_APB2ENR_TIM17EN             RCC_APBENR2_TIM17EN

// usart
#define RCC_APB2ENR_USART1EN            RCC_APBENR2_USART1EN
#define RCC_APB2ENR_USART2EN            RCC_APBENR2_USART2EN
#define USART_ISR_RXNE                  USART_ISR_RXNE_RXFNE
#define USART_ICR_NCF                   USART_ICR_NECF
#define USART_CR1_RXNEIE                USART_CR1_RXNEIE_RXFNEIE
#define USART_ISR_TXE                   USART_ISR_TXE_TXFNF

// dma
#define RCC_AHBENR_DMAEN                RCC_AHBENR_DMA1EN

// adc
#define ADC1_COMP_IRQn                  ADC1_IRQn
#define ADC_CFGR1_AWDEN                 ADC_CFGR1_AWD1EN
#define ADC_CFGR1_AWDSGL                ADC_CFGR1_AWD1SGL
#define ADC_CFGR1_AWDCH                 ADC_CFGR1_AWD1CH
#define ADC_CFGR1_AWDCH_0               ADC_CFGR1_AWD1CH_0
#define ADC_IER_AWDIE                   ADC_IER_AWD1IE
#define ADC_ISR_AWD                     ADC_ISR_AWD1
#define RCC_APB2ENR_ADC1EN              RCC_APBENR2_ADCEN

// dmamux: assignment of multiplexer inputs to resources
#define DMAMUX_REQ_MEM2MEM              0x00000000U
#define DMAMUX_REQ_GENERATOR0           0x00000001U
#define DMAMUX_REQ_GENERATOR1           0x00000002U
#define DMAMUX_REQ_GENERATOR2           0x00000003U
#define DMAMUX_REQ_GENERATOR3           0x00000004U
#define DMAMUX_REQ_ADC1                 0x00000005U
#define DMAMUX_REQ_I2C1_RX              0x0000000AU
#define DMAMUX_REQ_I2C1_TX              0x0000000BU
#define DMAMUX_REQ_I2C2_RX              0x0000000CU
#define DMAMUX_REQ_I2C2_TX              0x0000000DU
#define DMAMUX_REQ_SPI1_RX              0x00000010U
#define DMAMUX_REQ_SPI1_TX              0x00000011U
#define DMAMUX_REQ_SPI2_RX              0x00000012U
#define DMAMUX_REQ_SPI2_TX              0x00000013U
#define DMAMUX_REQ_TIM1_CH1             0x00000014U
#define DMAMUX_REQ_TIM1_CH2             0x00000015U
#define DMAMUX_REQ_TIM1_CH3             0x00000016U
#define DMAMUX_REQ_TIM1_CH4             0x00000017U
#define DMAMUX_REQ_TIM1_TRIG_COM        0x00000018U
#define DMAMUX_REQ_TIM1_UP              0x00000019U
#define DMAMUX_REQ_TIM3_CH1             0x00000020U
#define DMAMUX_REQ_TIM3_CH2             0x00000021U
#define DMAMUX_REQ_TIM3_CH3             0x00000022U
#define DMAMUX_REQ_TIM3_CH4             0x00000023U
#define DMAMUX_REQ_TIM3_TRIG            0x00000024U
#define DMAMUX_REQ_TIM3_UP              0x00000025U
#define DMAMUX_REQ_TIM16_CH1            0x0000002CU
#define DMAMUX_REQ_TIM16_COM            0x0000002DU
#define DMAMUX_REQ_TIM16_UP             0x0000002EU
#define DMAMUX_REQ_TIM17_CH1            0x0000002FU
#define DMAMUX_REQ_TIM17_COM            0x00000030U
#define DMAMUX_REQ_TIM17_UP             0x00000031U
#define DMAMUX_REQ_USART1_RX            0x00000032U
#define DMAMUX_REQ_USART1_TX            0x00000033U
#define DMAMUX_REQ_USART2_RX            0x00000034U
#define DMAMUX_REQ_USART2_TX            0x00000035U

// flash
#define FLASH_FKEY1                     ((uint32_t)0x45670123)  /*!< Flash program erase key1 */
#define FLASH_FKEY2                     ((uint32_t)0xCDEF89AB)  /*!< Flash program erase key2: used with FLASH_PEKEY1
                                                                    to unlock the write access to the FPEC. */

#define FLASH_ACR_PRFTBE                FLASH_ACR_PRFTEN
#define FLASH_SR_BSY                    FLASH_SR_BSY1

/**
 * @brief In the following line adjust the External High Speed oscillator (HSE) Startup
   Timeout value
   */
#if !defined  (HSE_STARTUP_TIMEOUT)
  #define HSE_STARTUP_TIMEOUT          ((uint16_t)0x5000) /*!< Time out for HSE start up */
#endif /* HSE_STARTUP_TIMEOUT */


int main()
{
    RCC->APBENR1 |= RCC_APBENR1_PWREN;

    RCC->AHBENR |= RCC_AHBENR_FLASHEN;

    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;


    GPIOA->MODER &= ~GPIO_MODER_MODE2_Msk;
    GPIOA->MODER |= GPIO_MODER_MODE2_0;

    GPIOA->MODER &= ~GPIO_MODER_MODE3_Msk;
    GPIOA->MODER |= GPIO_MODER_MODE3_0;

    GPIOA->BSRR = GPIO_BSRR_BR2;
    GPIOA->BSRR = GPIO_BSRR_BS3;


    // while (1) {
    //     GPIOA->BSRR = GPIO_BSRR_BR2;
    //     for (volatile unsigned i = 0; i < 1000; i++) {};
    //     GPIOA->BSRR = GPIO_BSRR_BS2;
    //     for (volatile unsigned i = 0; i < 1000; i++) {};
    // }

    while (1);
}
