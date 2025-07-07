#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "periph_gpio.h"
#include "irq_vectors.h"

#include "val_mod.h"

typedef struct {
    pclk_t pclk;
    uint8_t irqn;
    gpio_list_t gpio_list;
} adc_cfg_t;

const adc_cfg_t adc_cfg = {
    .pclk = PCLK_ADC1,
    .irqn = ADC1_2_IRQn,
    .gpio_list = {
        .count = 2,
        .pin_list = (const gpio_pin_t[]){
            { .port = GPIO_PORT_A, .pin = 0 },
            { .port = GPIO_PORT_A, .pin = 1 },
        },
        .cfg = {
            .mode = GPIO_MODE_ANALOG,
        }
    }
};

#define V_IN_PORT				GPIOA
#define V_IN_PIN				0
#define V_IN_CH					0

#define TANK_PORT				GPIOA
#define TANK_PIN				1
#define TANK_CH					1


#define FILTER_SIZE			16
uint16_t filter[2][FILTER_SIZE];
uint8_t findex = 0;

static void adc_handler(void)
{
    filter[V_IN_CH][findex] = ADC1->JDR2;
    filter[TANK_CH][findex] = ADC1->JDR1;
    findex++;
    if (findex == FILTER_SIZE) {
        findex = 0;
    }
    ADC1->SR &= ~ADC_SR_JEOC;
}

uint16_t adc_get(unsigned ch)
{
    unsigned val = 0;
    for (unsigned i = 0; i < FILTER_SIZE; i++) {
        val += filter[ch][i];
    }
    // так как фильтр домножает на 16 мы получаем за счет усреднения значение 1 ... 4095 * FILTER_SIZE = 65535
    return val;
}

void init_adc(void)
{
    const adc_cfg_t * cfg = &adc_cfg;

    pclk_ctrl(&cfg->pclk, 1);
    init_gpio_list(&cfg->gpio_list);

    ADC1->CR2 |= ADC_CR2_CAL;
    while (!(ADC1->CR2 & ADC_CR2_CAL));

    ADC1->SMPR2 |= (0b111 << (3 * V_IN_CH));
    ADC1->SMPR2 |= (0b111 << (3 * TANK_CH));	// 111: 239.5 cycles

    ADC1->CR2 |= ADC_CR2_JEXTSEL_0 * 0b111;		// External event select for injected group = 111: JSWSTART
    ADC1->CR2 |= ADC_CR2_JEXTTRIG;				// External trigger conversion mode for injected channels
    ADC1->CR2 |= ADC_CR2_CONT;					// Continuous conversion
    ADC1->CR1 |= ADC_CR1_SCAN;					// Scan mode
    ADC1->CR1 |= ADC_CR1_JAUTO; 				// Automatic Injected Group conversion

    ADC1->JSQR |= ADC_JSQR_JSQ4_0 * V_IN_CH;
    ADC1->JSQR |= ADC_JSQR_JSQ3_0 * TANK_CH;

    ADC1->JSQR |= ADC_JSQR_JL_0 * 1;			// Injected sequence length = 01: 2 conversions

    ADC1->CR1 |= ADC_CR1_JEOCIE;

    NVIC_EnableIRQ(cfg->irqn);
    NVIC_SetHandler(cfg->irqn, adc_handler);

    ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->CR2 |= ADC_CR2_JSWSTART;
}
