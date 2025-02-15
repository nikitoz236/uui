#include <stdint.h>
#include "config.h"

static uint8_t * rx_buf = 0;
static unsigned rx_len = 0;
static unsigned rx_idx = 0;

void kline_rx_byte_handler(char byte)
{
    if (rx_idx < rx_len) {
        rx_buf[rx_idx++] = byte;
    }
}

void kline_start_transaction(uint8_t * data, unsigned len, uint8_t * response, unsigned resp_len)
{
    rx_buf = response;
    rx_idx = 0;
    rx_len = resp_len + len;

    // usart_rx(&kline_usart, response, resp_len);
    usart_tx(&kline_usart, data, len);
}

unsigned kline_is_resp_available(void)
{
    // return usart_is_rx_available(&kline_usart);
    if (rx_len) {
        if (rx_len == rx_idx) {
            return 1;
        }
    }
    return 0;
}
