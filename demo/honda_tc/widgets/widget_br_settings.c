#include "ui_tree.h"
#include "text_label_color.h"
#include "draw_color.h"

#include "event_list.h"
#include "ui_mod.h"

void ui_set_brightness(unsigned br);

typedef struct {
    tf_ctx_t tf;
    unsigned state;
} ctx_t;

extern unsigned br;

extern const tf_cfg_t menu_title_cfg;

static const color_scheme_t br_cs = { .bg = COLOR(112233), .fg = COLOR(0x1144ff) };

static const label_t label_br = { .len = 2, .rep = { .vs = VAL_SIZE_32 }, .t = LP_V, .xy = { .x = -2 } };

static void apply_br(void * x)
{
    ui_set_brightness(br);
}

static const setting_mod_list_t br_mod = {
    .apply = apply_br,
    .count = 1,
    .mod_list = (const uv_mod_t []) {
        { .cfg = { .min = 1,    .max = 10,   .step = 1, }, .l = &label_br },
    }
};

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->state = 0;
    if (tf_ctx_calc(&ctx->tf, &el->f, &menu_title_cfg)) {
        draw_color_form(&el->f, br_cs.bg);
        lp(&ctx->tf, &label_br, &br_cs, 0, &br, 0);
        el->drawed = 1;
    }
}


// from widget_time_settings.c
void select_val(const tf_ctx_t * tf, const label_t * l, void * ctx, unsigned select);

static unsigned process(ui_element_t * el, unsigned event)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    unsigned new_state = ctx->state;
    const setting_mod_list_t * ml = &br_mod;
    const uv_mod_t * m = &ml->mod_list[ctx->state - 1];
    void * uv = &br;
    uv += ml->offset;

    if (ctx->state) {
        void * vp = uv + m->l->ofs;

        if (event == EVENT_BTN_UP) {
            val_mod(vp, m->l->rep, &m->cfg, MOD_OP_ADD);
            select_val(&ctx->tf, m->l, uv, 1);
            if (ml->change) {
                ml->change(ctx);
            }
            return 1;
        }

        if (event == EVENT_BTN_DOWN) {
            val_mod(vp, m->l->rep, &m->cfg, MOD_OP_SUB);
            select_val(&ctx->tf, m->l, uv, 1);
            if (ml->change) {
                ml->change(ctx);
            }
            return 1;
        }

        if (event == EVENT_BTN_BACK) {
            new_state--;
        }
    }
    if (event == EVENT_BTN_OK) {
        new_state++;
    }

    if (ctx->state != new_state) {
        if (new_state > ml->count) {
            new_state = 0;
            if (ml->apply) {
                ml->apply(ctx);
            }
        }

        if (new_state) {
            select_val(&ctx->tf, ml->mod_list[new_state - 1].l, uv, 1);
        }
        if (ctx->state) {
            select_val(&ctx->tf, m->l, uv, 0);
        } else {
            if (ml->prepare) {
                ml->prepare(ctx);
            }
        }
        ctx->state = new_state;
        return 1;
    }

    return 0;
}

const widget_desc_t widget_br_settings = {
    .draw = draw,
    .process_event = process,
    .ctx_size = sizeof(ctx_t)
};
