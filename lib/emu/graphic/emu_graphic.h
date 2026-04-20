#pragma once
#include "emu_lcd.h"
#include "api_lcd_color.h"

static inline void emu_graphic_init(emu_lcd_cfg_t * cfg)
{
    emu_init(cfg);
}

void emu_graphic_loop(void (*process)(char key));

/*
    цель - отладка графики
    2 режима. либо открыть окно с эмуляцией. либо считать битмап для отладки через ии агента.
    эмулируются как цветные так и чб экраны.


*/
