#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "periph_gpio.h"
#include "periph_spi.h"

#include "stm_usart.h"
#include "stm_pwm.h"

#include "lcd_spi.h"
#include "rtc.h"

extern const rcc_cfg_t rcc_cfg;
extern const usart_cfg_t debug_usart;
extern const usart_cfg_t kline_usart;
extern const lcd_cfg_t lcd_cfg;

