#include <stdint.h>
#include "config.h"

#define DP_NAME "K-LINE"
// #define DP_SILENT
#include "dp.h"

static uint8_t * rx_buf = 0;
static uint8_t rx_len = 0;
static uint8_t rx_idx = 0;
static uint8_t dbg_idx = 0;

void kline_rx_byte_handler(char byte)
{
    if (rx_idx < rx_len) {
        rx_buf[rx_idx++] = byte;
    }
}

void kline_start_transaction(uint8_t * data, unsigned len, uint8_t * response, unsigned resp_len)
{
    rx_idx = 0;
    dbg_idx = 0;
    rx_buf = response;
    rx_len = resp_len;

    // usart_rx(&kline_usart, response, resp_len);
    usart_tx(&kline_usart, data, len);
}

unsigned kline_is_resp_available(void)
{
    // return usart_is_rx_available(&kline_usart);
    while (dbg_idx < rx_idx) {
        dp("-> ");
        dpx(rx_buf[dbg_idx++], 1);
        dn();
    }
    if (rx_len) {
        if (rx_len == rx_idx) {
            return 1;
        }
    }
    return 0;
}
