#pragma once

struct pwm_cfg;
typedef const struct pwm_cfg pwm_cfg_t;

void pwm_set_freq(const pwm_cfg_t * cgf, unsigned f);
void pwm_set_ccr(const pwm_cfg_t * cgf, unsigned val);
void init_pwm(const pwm_cfg_t * cgf);
