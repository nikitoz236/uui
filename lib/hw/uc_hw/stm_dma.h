#include "periph_header.h"
#include "irq_vectors.h"
#include "bit_fields.h"

// Каналы нумерются от 1 как в даташите

static inline DMA_Channel_TypeDef * dma_channel(unsigned ch)
{
    if (ch == 0) {
        return 0;
    }
    const unsigned dma_step = DMA1_Channel2_BASE - DMA1_Channel1_BASE;
    return (DMA_Channel_TypeDef *)(DMA1_Channel1_BASE + (dma_step * (ch - 1)));
}

static inline void dma_set_periph_rx(unsigned ch, void * periph)
{
    DMA_Channel_TypeDef * dma = dma_channel(ch);
    dma->CCR &= ~DMA_CCR_DIR;
    dma->CPAR = (__IO uint32_t)periph;
}

static inline void dma_set_periph_tx(unsigned ch, void * periph)
{
    DMA_Channel_TypeDef * dma = dma_channel(ch);
    dma->CCR |= DMA_CCR_DIR;
    dma->CPAR = (__IO uint32_t)periph;
}

static inline void dma_start(unsigned ch, const void * ptr, unsigned len)
{
    DMA_Channel_TypeDef * dma = dma_channel(ch);
    dma->CMAR = (__IO uint32_t)ptr;
    dma->CNDTR = len;
    dma->CCR |= DMA_CCR_EN;
}

enum dma_size {
    DMA_SIZE_8 = 0,
    DMA_SIZE_16 = 1,
    DMA_SIZE_32 = 2,
};

static inline void dma_set_size(unsigned ch, enum dma_size size)
{
    DMA_Channel_TypeDef * dma = dma_channel(ch);
    dma->CCR &= ~DMA_CCR_EN;
    dma->CCR &= ~(DMA_CCR_PSIZE + DMA_CCR_MSIZE);
    dma->CCR |= (size * DMA_CCR_PSIZE_0) + (size * DMA_CCR_MSIZE_0);
}

static inline void dma_stop(unsigned ch)
{
    dma_channel(ch)->CCR &= ~DMA_CCR_EN;
}

static inline unsigned dma_get_cnt(unsigned ch)
{
    return dma_channel(ch)->CNDTR;
}

static inline unsigned dma_is_active(unsigned ch)
{
    if (dma_channel(ch)->CCR & DMA_CCR_EN) {
        return 1;
    }
    return 0;
}

static inline void dma_enable_nvic_irq(unsigned ch)
{
    NVIC_EnableIRQ(DMA1_Channel1_IRQn + ch - 1);
}

static inline void dma_set_handler(unsigned ch, void (*handler)(void))
{
    NVIC_SetHandler(DMA1_Channel1_IRQn + ch - 1, handler);
}

static inline unsigned dma_is_irq_full(unsigned ch)
{
    if (DMA1->ISR & (DMA_ISR_TCIF1 << (4 * (ch - 1)))) {
        return 1;
    }
    return 0;
}

static inline void dma_enable_mem_inc(unsigned ch)
{
    dma_channel(ch)->CCR |= DMA_CCR_MINC;
}

static inline void dma_disable_mem_inc(unsigned ch)
{
    dma_channel(ch)->CCR &= ~DMA_CCR_MINC;
}

static inline void dma_clear_irq_full(unsigned ch)
{
    DMA1->IFCR |= DMA_IFCR_CTCIF1 << (4 * (ch - 1));
}

static inline void dma_enable_irq_full(unsigned ch)
{
    dma_channel(ch)->CCR |= DMA_CCR_TCIE;
}

static inline void dma_disable_irq_full(unsigned ch)
{
    dma_channel(ch)->CCR &= ~DMA_CCR_TCIE;
}
