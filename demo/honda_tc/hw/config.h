#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_pwm.h"
#include "lcd_spi.h"

extern const hw_rcc_cfg_t hw_rcc_cfg;
extern const usart_cfg_t debug_usart;
extern const lcd_cfg_t lcd_cfg;

