#include "btn_debounce.h"
#include "bit_fields.h"

static const btn_debounce_cfg_t * cfg = 0;

void init_btn_debounce(const btn_debounce_cfg_t * c)
{
    cfg = c;
}

void btn_debounce_edge(unsigned ch, unsigned state)
{
    if (ch >= cfg->num) {
        return;
    }
    unsigned raw = bf_get(cfg->raw_state, ch, 1);
    if (raw == state) {
        return;
    }
    bf_set(cfg->raw_state, ch, 1, state);
    task_run_idx(cfg->task, ch, cfg->debounce_ms);
}

void __bdb_to(unsigned ch)
{
    unsigned raw = bf_get(cfg->raw_state, ch, 1);
    unsigned stable = bf_get(cfg->stable_state, ch, 1);
    if (raw != stable) {
        bf_set(cfg->stable_state, ch, 1, raw);
        cfg->on_edge(ch, raw);
    }
}
