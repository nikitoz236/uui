#include "pwm.h"
#include "mstimer.h"
#include "sound_subsystem.h"

#include "dp.h"

const pwm_cfg_t * pwm;
static sound_t * s;
static unsigned sidx;
mstimer_t st;

void sound_play(sound_t * sound)
{
    sidx = 0;
    s = sound;
    // sound_subsystem_process();
}

void init_sound_subsystem(const pwm_cfg_t * cfg)
{
    pwm = cfg;
    init_pwm(pwm);
    pwm_set_ccr(pwm, 0);
}

void sound_subsystem_process(void)
{
    if (s) {
        if (mstimer_is_over(&st)) {
            if (sidx < s->n) {
                pwm_set_freq(pwm, s->notes[sidx].freq);
                pwm_set_ccr(pwm, s->notes[sidx].vol);
                mstimer_start_timeout(&st, s->notes[sidx].ms_x10 * 10);
                sidx++;
            } else {
                pwm_set_ccr(pwm, 0);
                s = 0;
            }
        }
    }
}
