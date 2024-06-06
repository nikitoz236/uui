#pragma once
#include "str_val.h"

// должно быть обьявлено в проекте
void __debug_usart_tx_data(char * s, unsigned len);

static inline void dp(char * s)
{
    while (*s != 0) {
        __debug_usart_tx_data(s++, 1);
    }
}

static inline void dpn(char * s)
{
    dp(s);
    __debug_usart_tx_data("\r\n", 2);
}

static inline void dpd(unsigned d, unsigned w)
{
    char ds[16];
    dec_to_str_right_aligned(d, ds, w, 0);
    __debug_usart_tx_data(ds, w);

}

static inline void dpdz(unsigned d, unsigned w)
{
    char ds[16];
    dec_to_str_right_aligned(d, ds, w, 1);
    __debug_usart_tx_data(ds, w);
}
