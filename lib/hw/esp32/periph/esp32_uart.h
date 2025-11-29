#pragma once
#include "usart.h"
#include "pclk.h"
#include "gpio.h"

#include "soc/uart_struct.h"

struct usart_cfg {
    pclk_t usart;
    uart_dev_t * dev;
    gpio_t * tx_gpio;
    gpio_t * rx_gpio;
};
