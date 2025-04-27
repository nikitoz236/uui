#include "config.h"

#define WDG_KR_KEY_RELOAD 0xAAAA
#define BOOTLOADER_SIGNATURE    "mr6cuG"
const volatile char __attribute__((section (".section_signature"))) bootloader_signature[12] = BOOTLOADER_SIGNATURE;
const volatile uint32_t __attribute__((section (".section_flag"))) bootloader_flag = 0xAABBCCDD;

#include "stm_dma.h"

/*
    WS подключен к PB0
    TIM1_CH2N
    AF2
    DMA5 - TIM1_UP
*/

struct ws_cfg {
    TIM_TypeDef * tim;
    gpio_t gpio;
    pclk_t tim_pclk;
    uint8_t tim_ch;
    uint8_t dma_ch;
    uint8_t * buf;
};

uint8_t ws_frame_buf[24] = {
    3, 1, 1, 1, 1, 1, 1, 1,
    3, 3, 3, 3, 1, 1, 1, 1,
    1, 1, 1, 3, 3, 3, 3, 3
};

const struct ws_cfg ws_cfg = {
    .buf = ws_frame_buf,
    .dma_ch = 5,
    .tim_ch = 2 - 1,
    .gpio = {
        .cfg = {
            .mode = GPIO_MODE_AF,
            .type = GPIO_TYPE_PP,
            .speed = GPIO_SPEED_HIGH,
            .af = 2
        },
        .gpio = {
            .port = GPIO_PORT_B,
            .pin = 0
        }
    },
    .tim = TIM1,
    .tim_pclk = PCLK_TIM1
};


static void timer_configure_output(TIM_TypeDef * timer, unsigned ch, unsigned mode)
{
    uint8_t * ccmr_ch = (uint8_t*)&timer->CCMR1;
    if (ch & 2) {
        ccmr_ch += 4;
    }

    if (ch & 1) {
        ccmr_ch += 1;
    }

    *ccmr_ch |= TIM_CCMR1_CC1S_0 * 0;
    *ccmr_ch |= TIM_CCMR1_OC1M_0 * mode;
    timer->CCER |= 1 << (ch * 4);
}

// void pwm_set_ccr(const ws_cfg * cfg, unsigned val)
// {
//     __IO uint16_t * ccr = &cfg->tim->CCR1;
//     ccr[cfg->ch * 2] = val;
// }

void ws_dma_handler(void)
{
    const struct ws_cfg * cfg = &ws_cfg;

    dma_stop(cfg->dma_ch);

    cfg->tim->CCR2 = 0;

    cfg->tim->CR1 &= ~TIM_CR1_CEN;
    // gpio_set_state(&pin_cfg_2.gpio, 1);

    dma_clear_irq_full(cfg->dma_ch);
}

void ws_color_pack(uint8_t * buf, uint8_t color)
{
    for (uint8_t mask = 0x80; mask; mask >>= 1) {
        uint8_t bit = 1;
        if (color & mask) {
            bit = 3;
        }
        *buf++ = bit;
    }
}

void init_ws(const struct ws_cfg * cfg)
{
    pclk_ctrl(&cfg->tim_pclk, 1);
    init_gpio(&cfg->gpio);

    cfg->tim->BDTR |= TIM_BDTR_MOE;
    cfg->tim->CCER |= TIM_CCER_CC2NE;
    // cfg->tim->CCER |= TIM_CCER_CC2NP;
    cfg->tim->CCMR1 |= TIM_CCMR1_OC2M_0 * 6;

    cfg->tim->ARR = 4 - 1;
    cfg->tim->DIER |= TIM_DIER_UDE;

    // timer_configure_output(cfg->tim, cfg->tim_ch, 6);
    // cfg->tim->PSC = (hw_rcc_f_timer(&cfg->tim_pclk) / 4000000) - 1;


    // 110: PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1 else inactive.

    cfg->tim->PSC = 48 / 4 - 1;
    cfg->tim->CNT = 0;
    cfg->tim->CCR2 = 0;

    dma_channel(cfg->dma_ch)->CCR = 0;
    dma_enable_mem_inc(cfg->dma_ch);

    // dma_set_size(cfg->dma_ch, DMA_SIZE_16);
    dma_channel(cfg->dma_ch)->CCR |= DMA_CCR_PSIZE_0 * DMA_SIZE_16;
    dma_channel(cfg->dma_ch)->CCR |= DMA_CCR_MSIZE_0 * DMA_SIZE_8;

    dma_set_periph_tx(cfg->dma_ch, (void *)&cfg->tim->CCR2);

    dma_clear_irq_full(cfg->dma_ch);
    dma_enable_irq_full(cfg->dma_ch);
    // dma_set_handler(cfg->dma_ch, ws_dma_handler);
    // dma_enable_nvic_irq(cfg->dma_ch);

    NVIC_SetHandler(DMA1_Channel4_5_IRQn, ws_dma_handler);
    NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);
}

void ws_update(const struct ws_cfg * cfg)
{
    /*
        такой порядок позволяет сделать практически одновременной запись CCR через DMA и старт таймера
        иначе длительность импульса уползает

        также необходимо записать 0 в CCR2 в конце до остановки таймера. хотя это не так страшно. но импульс успевает вылететь
    */
    dma_start(cfg->dma_ch, cfg->buf, 24);
    cfg->tim->EGR |= TIM_EGR_UG;
    cfg->tim->CNT = 0;
    cfg->tim->CR1 |= TIM_CR1_CEN;
}

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);
    pclk_ctrl(&(pclk_t)PCLK_GPIOB, 1);
    pclk_ctrl(&(pclk_t)PCLK_DMA, 1);

    __enable_irq();

    // // init_pwm(ws2812_cfg.pwm_cfg);
    // // pwm_set_ccr(ws2812_cfg.pwm_cfg, 3);

    init_ws(&ws_cfg);

    ws_color_pack(&ws_cfg.buf[8],  32);
    ws_color_pack(&ws_cfg.buf[0],  16);
    ws_color_pack(&ws_cfg.buf[16], 0);

    for (volatile unsigned i = 0; i < 100000; i++) {};
    ws_update(&ws_cfg);

    while (1) {
        IWDG->KR = WDG_KR_KEY_RELOAD;
    };

    return 0;
}
