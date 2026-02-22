#include "esp32_gpio.h"
#include "esp32_spi.h"
#include "esp32_pwm.h"

#include "dbg_usb_cdc_acm.h"
#include "delay_blocking.h"

#define DP_NOTABLE
#include "dp.h"

// использовать ~/dev/esp32/hw_periph_dump_decode для чтения из порта

void __debug_usart_tx_data(const char * s, unsigned len)
{
    dbg_usb_cdc_acm_tx(s, len);
}

int main(void)
{
    dpn("t lora audio i2s test");

    while (1) {};
}
