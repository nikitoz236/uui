#pragma once
#include <stdint.h>

typedef struct {
    uint8_t addr;
    uint8_t bits;
    uint8_t bclk_div;
    uint8_t dac_osr;
    uint8_t adc_osr;
    uint16_t lrck_div;
} es8311_cfg_t;

void init_es8311(const es8311_cfg_t * cfg);
void es8311_set_mute(const es8311_cfg_t * cfg, unsigned mute);
void es8311_set_volume(const es8311_cfg_t * cfg, uint8_t vol);
uint8_t es8311_read_chipid(const es8311_cfg_t * cfg);
