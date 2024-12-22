#include "emu_common.h"
#include "widget_emu_lcd.h"

void emu_lcd_init(__widget_emu_lcd_cfg_t * cfg);
void emu_lcd_loop(void (*process)(char key));
