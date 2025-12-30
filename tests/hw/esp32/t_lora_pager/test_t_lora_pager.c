

#include "dbg_usb_cdc_acm.h"

#define DP_NOTABLE
#include "dp.h"

#include "soc/i2c_struct.h"
#include "delay_blocking.h"


void __debug_usart_tx_data(const char * s, unsigned len)
{
    dbg_usb_cdc_acm_tx(s, len);
}

int main(void)
{
    dpn("tlora, ok");


    while (1) {
        dpn("fuck");
        delay_ms(1000);
    }
}
