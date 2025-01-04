#include "stdio.h"
#include "dp.h"

void __debug_usart_tx_data(const char * s, unsigned len)
{
    while(len--) {
        printf("%c", *s++);
    }
}

int main()
{
    printf("debug print test\n");

    dp("fuck! this val: "); dpd(456, 5); dpn(" so ...");

    return 0;
}
