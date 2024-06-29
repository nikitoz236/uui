#pragma once
#include "str_val.h"
#include "str_utils.h"

// должно быть обьявлено в проекте
void __debug_usart_tx_data(const char * s, unsigned len);

// static const char * __debug_module_name = { __FILE__ };

static char __debug_is_str_started = 0;

unsigned __debug_is_module_silent(const char * name);

static inline void __debug_print_str(const char * s)
{
    __debug_usart_tx_data(s, str_len(s, -1));
}

static inline unsigned __debug_start(void)
{
    // if (__debug_is_module_silent()) {
    //     return 1;
    // }

    // if (__debug_is_str_started == 0) {

    //     __debug_usart_tx_data()
    // }
    return 0;
}

static inline void dp(const char * s)
{
    if (__debug_start()) {
        return;
    }
    __debug_print_str(s);
}

static inline void dpn(const char * s)
{
    if (__debug_start()) {
        return;
    }
    __debug_print_str(s);
    __debug_usart_tx_data("\r\n", 2);
    __debug_is_str_started = 0;
}

static inline void dn(const char * s)
{
    if (__debug_start()) {
        return;
    }
    __debug_usart_tx_data("\r\n", 2);
    __debug_is_str_started = 0;
}

static inline void dpd(unsigned d, unsigned w)
{
    if (__debug_start()) {
        return;
    }
    char ds[16];
    dec_to_str_right_aligned(d, ds, w, 0);
    __debug_usart_tx_data(ds, w);

}

static inline void dpdz(unsigned d, unsigned w)
{
    if (__debug_start()) {
        return;
    }
    char ds[16];
    dec_to_str_right_aligned(d, ds, w, 1);
    __debug_usart_tx_data(ds, w);
}

#define db dp
