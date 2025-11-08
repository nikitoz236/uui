#pragma once
#include "str_val.h"
#include "str_val_buf.h"
#include "str_utils.h"

/*
    #define DP_OFF
        - отключает вывод из этого модуля

    #define DP_NOTABLE
        - отключает табличный вывод, просто печатает как есть

    #define DP_SILENT
        - отключает вывод из этого модуля

    #define DP_NAME "name"
        - задает имя модуля, выводится в табличном виде
        - если не задано, то имя модуля не выводится
*/

#if defined DP_OFF
    #define dpcr()
    #define dpct(color)
    #define dpcb(color)
    #define dpl(s, l)
    #define dp(s)
    #define dpn(s)
    #define dn()
    #define dpd(d, w)
    #define dpdz(d, w)
    #define dpx(x, size)
    #define dpxd(x, size, count)
#else

// должно быть обьявлено в проекте
void __debug_usart_tx_data(const char * s, unsigned len);

// static const char * __debug_module_name = { __FILE__ };



enum {
    DPC_BLACK,
    DPC_RED,
    DPC_GREEN,
    DPC_YELLOW,
    DPC_BLUE,
    DPC_MAGENTA,
    DPC_CYAN,
    DPC_WHITE,
};

struct dp_ctx {
    struct dp_ctx * next;
    const char * name;
    unsigned silent : 4;
    unsigned started : 1;
    unsigned registred : 1;
    unsigned color : 1;
};

#ifndef DP_NAME
    #define DP_NAME 0
#endif


static struct dp_ctx __dp_ctx = {
    .name = DP_NAME,
    #ifdef DP_SILENT
        .silent = 1,
    #endif
};

static inline void __debug_print_str(const char * s, unsigned l, char padding_char)
{
    unsigned len = 0;
    if (s) {
        unsigned maxlen = l;
        if (maxlen == 0) {
            maxlen = (unsigned)-1;
        }
        len = str_len(s, maxlen);
        __debug_usart_tx_data(s, len);
    }

    while (len < l) {
        __debug_usart_tx_data(&padding_char, 1);
        len++;
    }
}

// функция проверят нужно ли выводить сообщения из модуля, если нужно то возвращает 0
// также печатает название модуля
static inline unsigned __debug_start(void)
{
    // if (__dp_ctx.name == 0) {
    //     // если не регистрировали модуль то пофиг. печатаем без названия как есть

    //     // позже здесь будет регистрация в системе логирования
    //     // а там и файлы всякие разные, и в кучу все можно валить и на флешку писать итд
    //     return 0;
    // }

    if (__dp_ctx.silent) {
        return 1;
    }

    #ifndef DP_NOTABLE

        #define NAME_LEN 10

        if (__dp_ctx.started == 0) {
            __dp_ctx.started = 1;
            if (__dp_ctx.color) {
                __debug_usart_tx_data("\x1b[m", 3);
                __dp_ctx.color = 0;
            }
            // __debug_usart_tx_data("[", 1);
            __debug_print_str(__dp_ctx.name, NAME_LEN, ' ');
            __debug_usart_tx_data("|", 2);
            // __debug_usart_tx_data("] ", 2);
        }
    #endif
    return 0;
}

static inline void dpcr(void)
{
    if (__debug_start()) {
        return;
    }
    __dp_ctx.color = 0;
    __debug_usart_tx_data("\x1b[m", 3);
}

static inline void dpct(unsigned color)
{
    if (__debug_start()) {
        return;
    }
    __dp_ctx.color = 1;
    char n = (char)('0' + color);
    __debug_usart_tx_data("\x1b[3", 3);
    __debug_usart_tx_data(&n, 1);
    __debug_usart_tx_data("m", 1);
}

static inline void dpcb(unsigned color)
{
    if (__debug_start()) {
        return;
    }
    __dp_ctx.color = 1;
    char n = (char)('0' + color);
    __debug_usart_tx_data("\x1b[4", 3);
    __debug_usart_tx_data(&n, 1);
    __debug_usart_tx_data("m", 1);
}


static inline void dpl(const char * s, unsigned l)
{
    if (__debug_start()) {
        return;
    }
    __debug_print_str(s, l, ' ');
}

static inline void dp(const char * s)
{
    if (__debug_start()) {
        return;
    }
    __debug_print_str(s, 0, 0);
}

static inline void dpn(const char * s)
{
    if (__debug_start()) {
        return;
    }
    __debug_print_str(s, 0, 0);
    __debug_usart_tx_data("\r\n", 2);
    __dp_ctx.started = 0;
}

static inline void dn(void)
{
    if (__debug_start()) {
        return;
    }
    __debug_usart_tx_data("\r\n", 2);
    __dp_ctx.started = 0;
}

// decimal
static inline void dpd(unsigned d, unsigned w)
{
    if (__debug_start()) {
        return;
    }
    char * str = str_val_buf_get();
    dec_to_str_right_aligned(d, str, w, 0);
    __debug_usart_tx_data(str, w);

}

// decimal with zero padding
static inline void dpdz(unsigned d, unsigned w)
{
    if (__debug_start()) {
        return;
    }
    char * str = str_val_buf_get();
    dec_to_str_right_aligned(d, str, w, 1);
    __debug_usart_tx_data(str, w);
}

// hex
static inline void dpx(unsigned x, unsigned size)
{
    if (__debug_start()) {
        return;
    }
    char * str = str_val_buf_get();
    hex_to_str(&x, str, size);
    __debug_usart_tx_data(str, size * 2);
}

// hex dump
static inline void dpxd(const void * x, unsigned size, unsigned count)
{
    if (__debug_start()) {
        return;
    }

    while (count--) {
        char * str = str_val_buf_get();
        hex_to_str(x, str, size);
        x += size;
        unsigned plen = size * 2;
        if (count) {
            str[plen] = ' ';
            plen++;
        }
        __debug_usart_tx_data(str, plen);
    }
}

#endif

// legacy redirector
#define db dp
