#include "stm32f10x.h"
#include "bit_fields.h"

static inline DMA_Channel_TypeDef * dma_channel(unsigned ch)
{
    const unsigned dma_step = DMA1_Channel2_BASE - DMA1_Channel1_BASE;
    return (DMA_Channel_TypeDef *)(DMA1_Channel1_BASE + (dma_step * ch));
}

static inline void dma_set_periph_rx(unsigned ch, void * periph)
{
    DMA_Channel_TypeDef * dma = dma_channel(ch);
    dma->CCR &= ~DMA_CCR1_DIR;
    dma->CPAR = (__IO uint32_t)periph;
}

static inline void dma_set_periph_tx(unsigned ch, void * periph)
{
    DMA_Channel_TypeDef * dma = dma_channel(ch);
    dma->CCR |= DMA_CCR1_DIR;
    dma->CPAR = (__IO uint32_t)periph;
}

static inline void dma_start(unsigned ch, void * ptr, unsigned len)
{
    DMA_Channel_TypeDef * dma = dma_channel(ch);
    dma->CMAR = (__IO uint32_t)ptr;
    dma->CNDTR = len;
    dma->CCR |= DMA_CCR1_EN;
}

static inline void dma_stop(unsigned ch)
{
    dma_channel(ch)->CCR &= ~DMA_CCR1_EN;
}

static inline unsigned dma_get_cnt(unsigned ch)
{
    return dma_channel(ch)->CNDTR;
}

static inline unsigned dma_is_active(unsigned ch)
{
    if (dma_channel(ch)->CCR & DMA_CCR1_EN) {
        return 1;
    }
    return 0;
}

void dma_set_handler(unsigned ch, void (*handler)(void));

static inline unsigned dma_is_irq_full(unsigned ch)
{
    if (DMA1->ISR & (DMA_ISR_TCIF1 << (4 * ch))) {
        return 1;
    }
    return 0;
}

static inline void dma_clear_irq_full(unsigned ch)
{
    DMA1->IFCR |= DMA_IFCR_CTCIF1 << (4 * ch);
}

static inline void dma_enable_irq_full(unsigned ch)
{
    dma_channel(ch)->CCR |= DMA_CCR1_TCIE;
}

static inline void dma_disable_irq_full(unsigned ch)
{
    dma_channel(ch)->CCR &= ~DMA_CCR1_TCIE;
}

