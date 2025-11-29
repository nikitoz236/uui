#include "esp32_uart.h"
#include "soc/system_struct.h"

void usart_set_cfg(const usart_cfg_t * cfg)
{
    if (cfg->tx_gpio) {
        init_gpio(cfg->tx_gpio);
    }

    if (cfg->rx_gpio) {
        init_gpio(cfg->rx_gpio);
    }

    if (SYSTEM.perip_clk_en0.uart_mem_clk_en == 0) {
        SYSTEM.perip_clk_en0.uart_mem_clk_en = 1;
    }

    pclk_ctrl(cfg->usart, 1);

    cfg->dev->clk_conf.rst_core = 1;
    cfg->dev->conf0.txfifo_rst = 1;
    cfg->dev->conf0.rxfifo_rst = 1;

    pclk_reset(cfg->usart);

    cfg->dev->clk_conf.rst_core = 0;
    cfg->dev->conf0.txfifo_rst = 0;
    cfg->dev->conf0.rxfifo_rst = 0;

    // cfg->dev->clkdiv.clkdiv = 173;

    cfg->dev->clkdiv.clkdiv = 10;

}

void usart_tx(const usart_cfg_t * usart, const void * data, unsigned len)
{
    while (len--) {
        while (usart->dev->status.txfifo_cnt == 127) {};
        // while (cfg->dev->int_raw.txfifo_empty_int_raw == 0) {};
        usart->dev->fifo.val = *(char *)data++;
    }
}

unsigned usart_is_tx_in_progress(const usart_cfg_t * usart)
{
    if (usart->dev->status.txfifo_cnt) {
        return 1;
    }
    return 0;
}
