#pragma once

struct backlight_cfg;
typedef const struct backlight_cfg backlight_cfg_t;

void init_backlight(backlight_cfg_t * cfg);
void bl_set(backlight_cfg_t * cfg, unsigned lvl);
