#pragma once
#include "usart.h"
#include "esp32_pclk.h"
#include "gpio.h"

#include "soc/uart_struct.h"

struct usart_cfg {
    unsigned baudrate;
    uart_dev_t * dev;
    gpio_t * tx_gpio;
    gpio_t * rx_gpio;
    pclk_t usart;
};
