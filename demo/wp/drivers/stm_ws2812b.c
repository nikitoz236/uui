#include "periph_header.h"
#include "stm_ws2812b.h"
#include "periph_gpio.h"

#include "stm_dma.h"

#define DBG_C(c)
        // wb_dc(c)

uint8_t ws_frame_buf[16] = {};
static const ws_cfg_t * ws_cfg = 0;
static const uint8_t * ws_colors_buf = 0;
static unsigned ws_byte_len = 0;
static unsigned ws_byte_counter = 0;

static void ws_color_pack(uint8_t * buf, uint8_t color)
{
    for (uint8_t mask = 0x80; mask; mask >>= 1) {
        uint8_t bit = 1;
        if (color & mask) {
            bit = 3;
        }
        *buf++ = bit;
    }
}

static void ws_dma_handler(void)
{
    DBG_C(0x90);
    DBG_C(ws_byte_counter);
    DBG_C(ws_cfg->tim->CCR2);

    uint8_t * buf;
    if (dma_is_irq(ws_cfg->dma_ch, DMA_IRQ_FULL)) {
            DBG_C(0x91);
        dma_clear_irq(ws_cfg->dma_ch, DMA_IRQ_FULL);
        buf = &ws_frame_buf[8];
    }
    if (dma_is_irq(ws_cfg->dma_ch, DMA_IRQ_HALF)) {
            DBG_C(0x92);
        dma_clear_irq(ws_cfg->dma_ch, DMA_IRQ_HALF);
        buf = &ws_frame_buf[0];
    }

    if ((ws_byte_counter + 2) < ws_byte_len) {
        ws_color_pack(buf, ws_colors_buf[ws_byte_counter + 2]);
            DBG_C(0x93);
    } else {
        uint32_t * ptr = (uint32_t *)buf;
        ptr[0] = 0;
        ptr[1] = 0;
            DBG_C(0x94);
    }

    ws_byte_counter++;
    if (ws_byte_counter >= (ws_byte_len + 1)) {
        ws_cfg->tim->CCR2 = 0;
        DBG_C(0x95);
        dma_stop(ws_cfg->dma_ch);
        ws_cfg->tim->CR1 &= ~TIM_CR1_CEN;
    }
}


// static void timer_configure_output(TIM_TypeDef * timer, unsigned ch, unsigned mode)
// {
//     uint8_t * ccmr_ch = (uint8_t*)&timer->CCMR1;
//     if (ch & 2) {
//         ccmr_ch += 4;
//     }

//     if (ch & 1) {
//         ccmr_ch += 1;
//     }

//     *ccmr_ch |= TIM_CCMR1_CC1S_0 * 0;
//     *ccmr_ch |= TIM_CCMR1_OC1M_0 * mode;
//     timer->CCER |= 1 << (ch * 4);
// }


void init_ws(const ws_cfg_t * cfg)
{
    ws_cfg = cfg;

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

    dma_channel(cfg->dma_ch)->CCR |= DMA_CCR_PSIZE_0 * DMA_SIZE_16;
    dma_channel(cfg->dma_ch)->CCR |= DMA_CCR_MSIZE_0 * DMA_SIZE_8;

    dma_set_periph_tx(cfg->dma_ch, (void *)&cfg->tim->CCR2);

    dma_clear_irq(cfg->dma_ch, DMA_IRQ_FULL);
    dma_set_irq_en(cfg->dma_ch, DMA_IRQ_FULL, 1);
    dma_clear_irq(cfg->dma_ch, DMA_IRQ_HALF);
    dma_set_irq_en(cfg->dma_ch, DMA_IRQ_HALF, 1);

    NVIC_SetHandler(ws_cfg->dma_irq, ws_dma_handler);
    NVIC_EnableIRQ(ws_cfg->dma_irq);
}

void ws_update(const ws_rgb_t * colors, unsigned len)
{
    ws_byte_len = len * 3;
    ws_byte_counter = 0;
    ws_colors_buf = (const uint8_t *)colors;
    ws_color_pack(&ws_frame_buf[0], ws_colors_buf[0]);
    ws_color_pack(&ws_frame_buf[8], ws_colors_buf[1]);

    /*
        такой порядок позволяет сделать практически одновременной запись CCR через DMA и старт таймера
        иначе длительность импульса уползает

        также необходимо записать 0 в CCR2 в конце до остановки таймера. хотя это не так страшно. но импульс успевает вылететь
    */

    dma_start(ws_cfg->dma_ch, ws_frame_buf, 16);
    ws_cfg->tim->EGR |= TIM_EGR_UG;
    ws_cfg->tim->CNT = 0;
    ws_cfg->tim->CR1 |= TIM_CR1_CEN;
}
