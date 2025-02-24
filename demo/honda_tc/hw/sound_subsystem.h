#pragma once
#include <stdint.h>

struct sound_note {
    uint16_t freq;
    uint8_t ms_x10;
    uint8_t vol;
};

typedef struct {
    struct sound_note * notes;
    uint16_t n;
} sound_t;

void sound_play(sound_t * sound);
void init_sound_subsystem(const pwm_cfg_t * cfg);
void sound_subsystem_process(void);
