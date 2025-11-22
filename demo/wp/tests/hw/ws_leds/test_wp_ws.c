#include "periph_header.h"
#include "periph_rcc.h"
#include "periph_pclk.h"
#include "periph_gpio.h"

// #include "stm_ws2812.h"

const rcc_cfg_t rcc_cfg = {
    .hse_val = 8000000,
    .pll_src = PLL_SRC_PREDIV,
    .pll_prediv = 1,
    .pll_mul = 6,
    .sysclk_src = SYSCLK_SRC_PLL,
    .hclk_div = HCLK_DIV1,
    .apb_div = {
        APB_DIV1,
    }
};

const gpio_list_t dbg_pins = {
    .count = 5,
    .pin_list = (gpio_pin_t []){
        { .port = GPIO_PORT_B, .pin = 3},
        { .port = GPIO_PORT_B, .pin = 5},
        { .port = GPIO_PORT_B, .pin = 4},
        { .port = GPIO_PORT_A, .pin = 12},
        { .port = GPIO_PORT_A, .pin = 15},
    },
    .cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_HIGH,
    }
};

const gpio_t status_led = {
    .gpio = {
        .port = GPIO_PORT_A,
        .pin = 5
    },
    .cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_HIGH,
    }
};











gpio_t dbg_spi_pin = {
    .gpio = { .port = GPIO_PORT_A, .pin = 14 },
    .cfg = { .mode = GPIO_MODE_AF, .af = 6 }
};

#define DBG_SPI SPI2

void init_dbg_spi_usart(void)
{
    pclk_ctrl(&(pclk_t)PCLK_SPI2, 1);
    init_gpio(&dbg_spi_pin);

    DBG_SPI->CR2 |= SPI_CR2_DS_0 * 0x0F;
    DBG_SPI->CR2 &= (~SPI_CR2_DS) | (SPI_CR2_DS_0 * (11 - 1));

    DBG_SPI->CR1 =
        SPI_CR1_MSTR +          // Master configuration
        SPI_CR1_SSM +           // Software slave management
        SPI_CR1_SSI +           // Internal slave select
        // SPI_CR2_SSOE +
        (SPI_CR1_BR_0 * 3) +
        SPI_CR1_LSBFIRST +
        0;

    DBG_SPI->CR1 |= SPI_CR1_SPE;
}

void wb_dc(char c)
{
    while(!(SPI2->SR & SPI_SR_TXE)) {};
    uint32_t x = c << 2;
    x |= 1 + (1 << 10);
    SPI2->DR = x;
}

void wb_dpl(const void *x, unsigned l)
{
    const char * s = (const char *)x;
    while (l--) {
        wb_dc(*s++);
    }
}









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

uint8_t ws_frame_buf[16] = {};

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

struct rgb {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct rgb ws_colors[16] = {

    { .r = 1, .g = 0, .b = 11 },
    { .r = 0, .g = 1, .b = 1 },
    { .r = 1, .g = 2, .b = 0 },
    { .r = 2, .g = 0, .b = 1 },
    { .r = 32, .g = 0, .b = 0 },
    { .r = 0, .g = 32, .b = 0 },
    { .r = 0, .g = 0, .b = 32 },
    { .r = 16, .g = 16, .b = 0 },
    { .r = 32, .g = 0, .b = 0 },
    { .r = 0, .g = 16, .b = 0 },
    { .r = 0, .g = 0, .b = 16 },
    { .r = 16, .g = 16, .b = 0 },
    { .r = 16, .g = 0, .b = 16 },
    { .r = 0, .g = 16, .b = 16 },
    { .r = 16, .g = 32, .b = 0 },
    { .r = 32, .g = 0, .b = 16 },

};

volatile unsigned ws_byte_len = 0;
volatile unsigned ws_byte_counter = 0;

#define DBG_C(c)
        // wb_dc(c)

void ws_dma_handler(void)
{
    static const struct ws_cfg * cfg = &ws_cfg;
    DBG_C(0x90);
    DBG_C(ws_byte_counter);
    DBG_C(cfg->tim->CCR2);

    uint8_t * buf;
    if (dma_is_irq(cfg->dma_ch, DMA_IRQ_FULL)) {
            DBG_C(0x91);
        dma_clear_irq(cfg->dma_ch, DMA_IRQ_FULL);
        buf = &cfg->buf[8];
    }
    if (dma_is_irq(cfg->dma_ch, DMA_IRQ_HALF)) {
            DBG_C(0x92);
        dma_clear_irq(cfg->dma_ch, DMA_IRQ_HALF);
        buf = &cfg->buf[0];
    }

    if ((ws_byte_counter + 2) < ws_byte_len) {
        const uint8_t * ws_bytes = (const uint8_t *)ws_colors;
        ws_color_pack(buf, ws_bytes[ws_byte_counter + 2]);
            DBG_C(0x93);
    } else {
        uint32_t * ptr = (uint32_t *)buf;
        ptr[0] = 0;
        ptr[1] = 0;
            DBG_C(0x94);
    }

    ws_byte_counter++;
    if (ws_byte_counter >= (ws_byte_len + 1)) {
        cfg->tim->CCR2 = 0;
        DBG_C(0x95);
        dma_stop(ws_cfg.dma_ch);
        cfg->tim->CR1 &= ~TIM_CR1_CEN;
    }
}

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
    dma_channel(cfg->dma_ch)->CCR |= DMA_CCR_CIRC;

    dma_enable_mem_inc(cfg->dma_ch);

    // dma_set_size(cfg->dma_ch, DMA_SIZE_16);
    dma_channel(cfg->dma_ch)->CCR |= DMA_CCR_PSIZE_0 * DMA_SIZE_16;
    dma_channel(cfg->dma_ch)->CCR |= DMA_CCR_MSIZE_0 * DMA_SIZE_8;

    dma_set_periph_tx(cfg->dma_ch, (void *)&cfg->tim->CCR2);

    dma_clear_irq(cfg->dma_ch, DMA_IRQ_FULL);
    dma_set_irq_en(cfg->dma_ch, DMA_IRQ_FULL, 1);
    dma_clear_irq(cfg->dma_ch, DMA_IRQ_HALF);
    dma_set_irq_en(cfg->dma_ch, DMA_IRQ_HALF, 1);

    // dma_set_handler(cfg->dma_ch, ws_dma_handler);
    // dma_enable_nvic_irq(cfg->dma_ch);

    NVIC_SetHandler(DMA1_Channel4_5_IRQn, ws_dma_handler);
    NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);
}

void ws_update(const struct ws_cfg * cfg, struct rgb * colors, unsigned len)
{
    ws_byte_len = len * 3;
    ws_byte_counter = 0;
    /*
        такой порядок позволяет сделать практически одновременной запись CCR через DMA и старт таймера
        иначе длительность импульса уползает

        также необходимо записать 0 в CCR2 в конце до остановки таймера. хотя это не так страшно. но импульс успевает вылететь
    */
    const uint8_t * ws_bytes = (const uint8_t *)colors;
    ws_color_pack(&cfg->buf[0], ws_bytes[0]);
    ws_color_pack(&cfg->buf[8], ws_bytes[1]);

    dma_start(cfg->dma_ch, cfg->buf, 16);
    cfg->tim->EGR |= TIM_EGR_UG;
    cfg->tim->CNT = 0;
    cfg->tim->CR1 |= TIM_CR1_CEN;
}

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);
    pclk_ctrl(&(pclk_t)PCLK_GPIOA, 1);
    pclk_ctrl(&(pclk_t)PCLK_GPIOB, 1);
    pclk_ctrl(&(pclk_t)PCLK_DMA, 1);

    init_dbg_spi_usart();

    __enable_irq();

    // // init_pwm(ws2812_cfg.pwm_cfg);
    // // pwm_set_ccr(ws2812_cfg.pwm_cfg, 3);

    init_ws(&ws_cfg);

    DBG_C(0x5E);
    for (volatile unsigned i = 0; i < 100000; i++) {};
    ws_update(&ws_cfg, ws_colors, 16);

    while (1) {};

    return 0;
}
