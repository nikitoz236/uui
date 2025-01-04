#pragma once
#include "str_val.h"
#include "str_val_buf.h"
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

// функция проверят нужно ли выводить сообщения из модуля, если нужно то возвращает 0
// также печатает название модуля
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

static inline void dn(void)
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
    char * str = str_val_buf_get();
    dec_to_str_right_aligned(d, str, w, 1);
    str_val_buf_lock();
    __debug_usart_tx_data(str, w);
}

static inline void dpx(unsigned x, unsigned size)
{
    if (__debug_start()) {
        return;
    }
    char * str = str_val_buf_get();
    hex_to_str(&x, str, size);
    str_val_buf_lock();
    __debug_usart_tx_data(str, size * 2);
}

static inline void dpxd(const void * x, unsigned size, unsigned count)
{
    if (__debug_start()) {
        return;
    }

    char * str = str_val_buf_get();
    while (count--) {
        hex_to_str(x, str, size);
        x += size;
        unsigned plen = size * 2;
        if (count) {
            str[plen] = ' ';
            plen++;
        }
        str_val_buf_lock();
        __debug_usart_tx_data(str, plen);
        // здесь есть проблема, если мы передаем через DMA без промежуточного буфера, то следующий цикл изменит str
        // до его полной отправки предыдущим циклом, так как __debug_usart_tx_data возвращает управление до полной отправки
    }
}

#define db dp
