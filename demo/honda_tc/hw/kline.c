#include <stdint.h>
#include "config.h"

static uint8_t * rx_buf = 0;
static unsigned rx_len = 0;

void kline_rx_byte_handler(char byte)
{
    if (rx_len) {
        *rx_buf = byte;
        rx_buf++;
        rx_len--;
    }
}

void kline_start_transaction(uint8_t * data, unsigned len, uint8_t * response, unsigned resp_len)
{
    rx_buf = response;
    rx_len = resp_len;

    // usart_rx(&kline_usart, response, resp_len);
    usart_tx(&kline_usart, data, len);
}

unsigned kline_is_resp_available(void)
{
    // return usart_is_rx_available(&kline_usart);
    if (rx_len) {
        return 0;
    }
    return 1;
}
