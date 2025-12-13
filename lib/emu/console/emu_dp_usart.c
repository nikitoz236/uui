#include "stdio.h"

void __debug_usart_tx_data(const char * s, unsigned len)
{
    while(len--) {
        printf("%c", *s++);
    }
    fflush(stdout);
}
