#pragma once
#include <stdint.h>

typedef uint8_t pclk_t;

void pclk_ctrl(pclk_t pclk, unsigned state);
void pclk_reset(pclk_t pclk);
