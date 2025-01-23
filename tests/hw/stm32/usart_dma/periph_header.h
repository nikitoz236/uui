#include "stm32f10x.h"

#define RDR DR
#define TDR DR

#define USART_ISR_TXE       USART_SR_TXE

#define DMA_CCR_EN          DMA_CCR1_EN
#define DMA_CCR_TCIE        DMA_CCR1_TCIE
#define DMA_CCR_HTIE        DMA_CCR1_HTIE
#define DMA_CCR_TEIE        DMA_CCR1_TEIE
#define DMA_CCR_DIR         DMA_CCR1_DIR
#define DMA_CCR_CIRC        DMA_CCR1_CIRC
#define DMA_CCR_PINC        DMA_CCR1_PINC
#define DMA_CCR_MINC        DMA_CCR1_MINC

#define DMA_CCR_PSIZE       DMA_CCR1_PSIZE
#define DMA_CCR_PSIZE_0     DMA_CCR1_PSIZE_0
#define DMA_CCR_MSIZE       DMA_CCR1_MSIZE
#define DMA_CCR_MSIZE_0     DMA_CCR1_MSIZE_0
