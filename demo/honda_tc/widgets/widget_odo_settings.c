#include "widget_odo_settings.h"
#include "text_label_color.h"
#include "ui_mod.h"
#include "draw_color.h"

#include "event_list.h"
#include "routes.h"


typedef struct {
    uint8_t digit[7];
    unsigned val;
} uv_t;

typedef struct {
    tf_ctx_t tf;
    uv_t uv;
    unsigned state;
} ctx_t;

extern const tf_cfg_t menu_title_cfg;

static const lcd_color_t bg = COLOR(112233);

static void calc_digits_ctx(uv_t * uv, unsigned x)
{
    for (int i = 6; i >= 0; i--) {
        uv->digit[i] = x % 10;
        x /= 10;
    }
}

static void load_odo_shift_ctx(uv_t * uv, unsigned x)
{
    (void)x;

    uv->val = route_get_start_odo() / 1000;
}

static const label_t labels_digit[] = {
    { .t = LP_V, .xy = { .x = -8 }, .rep = { .vs = VAL_SIZE_8 }, .len = 1, .vt = { .zl = 1 }, .ofs = 0 },
    { .t = LP_V, .xy = { .x = -7 }, .rep = { .vs = VAL_SIZE_8 }, .len = 1, .vt = { .zl = 1 }, .ofs = 1 },
    { .t = LP_V, .xy = { .x = -6 }, .rep = { .vs = VAL_SIZE_8 }, .len = 1, .vt = { .zl = 1 }, .ofs = 2 },
    { .t = LP_V, .xy = { .x = -5 }, .rep = { .vs = VAL_SIZE_8 }, .len = 1, .vt = { .zl = 1 }, .ofs = 3 },
    { .t = LP_V, .xy = { .x = -4 }, .rep = { .vs = VAL_SIZE_8 }, .len = 1, .vt = { .zl = 1 }, .ofs = 4 },
    { .t = LP_V, .xy = { .x = -3 }, .rep = { .vs = VAL_SIZE_8 }, .len = 1, .vt = { .zl = 1 }, .ofs = 5 },
    { .t = LP_V, .xy = { .x = -2 }, .rep = { .vs = VAL_SIZE_8 }, .len = 1, .vt = { .zl = 1 }, .ofs = 6 },
};

static const label_list_t ll_digits = {
    .wrap_list = (label_color_t []) {
        {
            .color = COLOR(0x1144ff),
            .l = {
                .t = LP_S, .rep = { .vs = VAL_SIZE_32 }, .sofs = 0, .ofs = offsetof(uv_t, val),
                .sl = &(const label_list_t){ .ctx_update = (void(*)(void *, unsigned))calc_digits_ctx, .list = labels_digit, .count = 7 }
            }
        },
    },
    .count = 1,
    .ctx_update = (void(*)(void *, unsigned))load_odo_shift_ctx
};

static void mod_apply_odo(ctx_t * ctx)
{
    ctx->uv.val = 0;
    unsigned factor = 1;
    for (int i = 6; i >= 0; i--) {
        ctx->uv.val += ctx->uv.digit[i] * factor;
        factor *= 10;
    }
    route_set_start_odo(ctx->uv.val * 1000);
}

static const setting_mod_list_t odo_mod = {
    .apply = (void (*)(void *))mod_apply_odo,
    .count = 7,
    .offset = offsetof(ctx_t, uv.digit),
    .mod_list = (const uv_mod_t []) {
        { .cfg = { .min = 0,    .max = 3,   .step = 1, .ovf = 1 }, .l = &labels_digit[0] },
        { .cfg = { .min = 0,    .max = 9,   .step = 1, .ovf = 1 }, .l = &labels_digit[1] },
        { .cfg = { .min = 0,    .max = 9,   .step = 1, .ovf = 1 }, .l = &labels_digit[2] },
        { .cfg = { .min = 0,    .max = 9,   .step = 1, .ovf = 1 }, .l = &labels_digit[3] },
        { .cfg = { .min = 0,    .max = 9,   .step = 1, .ovf = 1 }, .l = &labels_digit[4] },
        { .cfg = { .min = 0,    .max = 9,   .step = 1, .ovf = 1 }, .l = &labels_digit[5] },
        { .cfg = { .min = 0,    .max = 9,   .step = 1, .ovf = 1 }, .l = &labels_digit[6] },
    }
};

static void draw(ui_element_t * el)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    ctx->state = 0;
    if (tf_ctx_calc(&ctx->tf, &el->f, &menu_title_cfg)) {
        draw_color_form(&el->f, bg);
        label_color_list(&ctx->tf, &ll_digits, bg, 0, &ctx->uv, 0);
        el->drawed = 1;
    }
}

// from widget_time_settings.c
void select_val(const tf_ctx_t * tf, const label_t * l, void * ctx, unsigned select);

static unsigned process(ui_element_t * el, unsigned event)
{
    ctx_t * ctx = (ctx_t *)el->ctx;
    unsigned new_state = ctx->state;
    const setting_mod_list_t * ml = &odo_mod;
    const uv_mod_t * m = &ml->mod_list[ctx->state - 1];
    void * uv = ctx;
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

const widget_desc_t widget_odo_settings = {
    .draw = draw,
    .process_event = process,
    .ctx_size = sizeof(ctx_t),
};
