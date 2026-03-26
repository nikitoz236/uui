#pragma once
#include <stdint.h>
#include "soc/gdma_struct.h"
#include "soc/system_reg.h"
#include "esp32_pclk.h"

/* ── DMA дескриптор (linked list item) ───────────────── */

typedef struct gdma_desc {
    volatile uint32_t size     : 12; /* размер буфера buf в байтах (max 4095) */
    volatile uint32_t length   : 12; /* TX: сколько байт передать. RX: заполняется контроллером */
    volatile uint32_t offset   : 5;  /* смещение внутри buf */
    volatile uint32_t sosf     : 1;  /* start of sub-frame */
    volatile uint32_t eof      : 1;  /* end of frame — генерирует прерывание, не останавливает DMA */
    volatile uint32_t owner    : 1;  /* 1 = DMA владеет дескриптором, 0 = CPU */
    void * buf;
    struct gdma_desc * next;         /* следующий дескриптор, NULL = конец цепочки */
} gdma_desc_t;

/* ── инициализация контроллера GDMA ──────────────────── */

static inline void gdma_init(void)
{
    pclk_ctrl(32 + SYSTEM_DMA_CLK_EN_S, 1);
    pclk_reset(32 + SYSTEM_DMA_CLK_EN_S);
    GDMA.misc_conf.clk_en = 1;
}

/* ── запуск TX-канала ────────────────────────────────── */

static inline void gdma_tx_start(unsigned ch, unsigned peri_sel, const gdma_desc_t * desc)
{
    GDMA.channel[ch].out.conf0.out_rst = 1;
    GDMA.channel[ch].out.conf0.out_rst = 0;
    GDMA.channel[ch].out.peri_sel.sel = peri_sel;
    GDMA.channel[ch].out.link.val = ((uint32_t)desc & 0xFFFFF) | (1 << 21);
}
