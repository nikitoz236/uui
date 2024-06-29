#pragma once

struct usart_cfg;

typedef struct usart_cfg usart_cfg_t;

void usart_set_cfg(const usart_cfg_t * usart);
void usart_set_baud(const usart_cfg_t * usart, unsigned len);

void usart_tx(const usart_cfg_t * usart, const void * data, unsigned len);
unsigned usart_is_tx_in_progress(const usart_cfg_t * usart);

unsigned usart_is_rx_available(const usart_cfg_t * usart);
void usart_rx(const usart_cfg_t * usart, void * data, unsigned len);
