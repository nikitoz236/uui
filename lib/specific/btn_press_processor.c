#include "btn_press_processor.h"
#include "eq.h"
#include "bit_fields.h"

static const btn_press_processor_cfg_t * cfg = 0;

void init_btn_press_processor(const btn_press_processor_cfg_t * c)
{
    cfg = c;
}

static void emit_short(unsigned ch)
{
    cfg->on_event(ch, PRESS_SHORT);
}

static void emit_edge(unsigned ch)
{
    if (bf_get(cfg->input_state, ch, 1)) {
        cfg->on_event(ch, PRESS_ON);
    } else {
        cfg->on_event(ch, PRESS_OFF);
    }
}

void btn_press_processor_edge(unsigned ch, unsigned state)
{
    if (ch >= cfg->num) {
        return;
    }
    bf_set(cfg->input_state, ch, 1, state);

    if (state) {
        // press
        task_run_idx(cfg->task_long, ch, cfg->long_ms);
        if (cfg->task_double) {
            unsigned act = task_is_active_idx(cfg->task_double, ch);
            bf_set(cfg->double_pressed, ch, 1, act);
        }
    } else {
        // release
        unsigned long_pressed = bf_get(cfg->long_pressed, ch, 1);
        if (!long_pressed) {
            if (cfg->task_double) {
                unsigned dbl = bf_get(cfg->double_pressed, ch, 1);
                unsigned delay = cfg->double_ms;
                if (dbl) {
                    delay = 0;
                }
                task_run_idx(cfg->task_double, ch, delay);
            } else {
                eq_func_idx(emit_short, ch);
            }
        }
        task_stop_idx(cfg->task_long, ch);
        bf_set(cfg->long_pressed, ch, 1, 0);
    }

    eq_func_idx(emit_edge, ch);
}

void __bpp_long_to(unsigned ch)
{
    unsigned long_pressed = bf_get(cfg->long_pressed, ch, 1);
    unsigned dbl = 0;
    if (cfg->double_pressed) {
        dbl = bf_get(cfg->double_pressed, ch, 1);
    }

    if (!long_pressed) {
        bf_set(cfg->long_pressed, ch, 1, 1);
        if (dbl) {
            cfg->on_event(ch, PRESS_SHORT_LONG);
        } else {
            cfg->on_event(ch, PRESS_LONG);
        }
    }
    if (cfg->repeat_ms) {
        if (dbl) {
            cfg->on_event(ch, PRESS_SHORT_LONG_REPEAT);
        } else {
            cfg->on_event(ch, PRESS_LONG_REPEAT);
        }
        task_run_idx(cfg->task_long, ch, cfg->repeat_ms);
    }
}

void __bpp_dbl_to(unsigned ch)
{
    // ещё держат - SHORT/DOUBLE эмитим только после release
    if (bf_get(cfg->input_state, ch, 1)) {
        return;
    }
    unsigned dbl = bf_get(cfg->double_pressed, ch, 1);
    if (dbl) {
        cfg->on_event(ch, PRESS_DOUBLE);
    } else {
        cfg->on_event(ch, PRESS_SHORT);
    }
}
