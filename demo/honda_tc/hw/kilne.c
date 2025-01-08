#include <stdint.h>
#include "config.h"


void kline_start_transaction(uint8_t * data, unsigned len, uint8_t * response, unsigned resp_len)
{
    usart_rx(&kline_usart, response, resp_len);
}

unsigned kline_is_resp_available(void)
{

}
