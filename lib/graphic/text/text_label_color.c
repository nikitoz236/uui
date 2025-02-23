#include "str_val_buf.h"
#include "text_label_color.h"
#include "lcd_text_color.h"
#include "str_utils.h"

// #include <stdio.h>

void label_static_print(const tf_ctx_t * tf, const label_static_t * l, color_scheme_t * cs, unsigned idx)
{
    const char * str;
    if (l->to_str) {
        str = l->to_str(idx);
    } else {
        str = l->text;
    }
    lcd_color_text_raw_print(str, tf->tfcfg->fcfg, cs, &tf->xy, &tf->tfcfg->limit_char, &l->xy, l->len);
}

static unsigned update_ctx_val(void * ctx, void * new, val_rep_t rep, unsigned offset)
{
    static const uint8_t vl[] = {
        [VAL_SIZE_8] = 1,
        [VAL_SIZE_16] = 2,
        [VAL_SIZE_32] = 4,
    };
    if (!str_cmp(ctx + offset, new + offset, vl[rep.vs])) {
        str_cp(ctx + offset, new + offset, vl[rep.vs]);
        return 1;
    }
    return 0;
}

void label_value_print(const tf_ctx_t * tf, const label_value_t * l, color_scheme_t * cs, void * prev_ctx, void * ctx)
{
    // идея в двух контекстах

    if (prev_ctx) {
        if (!update_ctx_val(prev_ctx, ctx, l->rep, l->ofs)) {
            return;
        }
    }

    unsigned t = l->t;
    unsigned v;

    if (t != LV) {
        v = val_unpack(ctx + l->ofs, l->rep, 0);
    }

    if (t == LS) {
        void * sub_ctx = ctx + l->sofs;

        l->sl->ctx_update(sub_ctx, v);
        void * sub_prev_ctx = 0;
        if (prev_ctx) {
            sub_prev_ctx = prev_ctx + l->sofs;
        }
        for (unsigned i = 0; i < l->sl->count; i++) {
            label_value_print(tf, &l->sl->list[i], cs, sub_prev_ctx, sub_ctx);
        }
    } else {
        const char * str;
        if (l->t == LF) {
            str = l->to_str(v);
        } else {
            str = str_val_buf_get();
            val_text_ptr_to_str((char *)str, ctx + l->ofs, l->rep, l->vt, l->len);
        }
        lcd_color_text_raw_print(str, tf->tfcfg->fcfg, cs, &tf->xy, &tf->tfcfg->limit_char, &l->xy, 0);
    }
}
