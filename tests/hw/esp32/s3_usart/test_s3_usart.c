#include "esp32_uart.h"
#include "esp32_gpio.h"

#define DP_NOTABLE
#include "dp.h"

usart_cfg_t uart2_cfg = {
    .baudrate = 115200,
    .usart = SYSTEM_UART2_CLK_EN_S + 32,
    .dev = &UART2,
    .tx_gpio = &(gpio_t){
        .cfg = { .mode = GPIO_MODE_SIG_OUT },
        .pin = { .pin = 1, .signal = U2TXD_OUT_IDX }
    }
};

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&uart2_cfg, s, len);
}

int main(void)
{
    usart_set_cfg(&uart2_cfg);
    dpn("Hey bitch!");



    while (1) {};
}


