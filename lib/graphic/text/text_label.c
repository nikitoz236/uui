#include "text_label.h"
#include "str_val_buf.h"
#include "str_utils.h"

#include "lcd_text_color.h"

#include "dp.h"

typedef struct {
    tptr_t * tp;
    void * uv;
    void * uv_prev;
} label_process_ctx_t;

static lcd_color_t * color_palette;
label_process_ctx_t label_process_ctx;

void update_ctx(void * ctx, void * val, val_rep_t rep, ctx_update_func_t * f)
{
    if (rep.vs = VAL_SIZE_PTR) {
        f->p(ctx, val);
    } else {
        if (rep.s) {
            int v = val_ptr_to_signed(val, rep.vs);
            f->s(ctx, v);
        } else {
            unsigned v = val_ptr_to_usnigned(val, rep.vs);
            f->u(ctx, v);
        }
    }
}

static unsigned ctx_check_val(void * ctx, void * new, val_rep_t rep, unsigned offset)
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

static const char * str_from_val(void * ptr, val_rep_t rep, val_text_t vt, unsigned len)
{
    const char * str = str_val_buf_get();
    val_text_ptr_to_str((char *)str, ptr, rep, vt, len);
    return str;
}

static void label_process(label_t * l);

void label_sublist_proces(label_t * l)
{
    label_sublist_t * sl = &l->sublist;
    void * val_ptr = label_process_ctx.uv + l->val_offset_in_ctx;

    label_process_ctx_t stored = label_process_ctx;
    unsigned sub_ctx_offset = l->sub_list_ctx_offset;

    label_process_ctx.uv += sub_ctx_offset;
    if (label_process_ctx.uv_prev) {
        label_process_ctx.uv_prev += sub_ctx_offset;
    }

    update_ctx(label_process_ctx.uv, val_ptr, l->val_rep, &sl->update_func);

    tptr_t sub_tp = text_ptr_export(label_process_ctx.tp, (xy_t){});
    label_process_ctx.tp = &sub_tp;

    list_process_all(sl->labels, label_process);

    label_process_ctx = stored;
}

// нам нужно еще относительное положение
static void label_process(label_t * l)
{
    dp("process label "); dpx((unsigned)l, 4); dp(" : "); dpxd(l, 1, sizeof(label_t)); dn();
    dp("xy "); dpd(l->pos.x, 3); dp(","); dpd(l->pos.y, 3); dp(" type "); dpd(l->type, 1); dn();
    text_ptr_set_char_pos(label_process_ctx.tp, l->pos);
    if (l->type == LABEL_SUB_LIST) {
        label_sublist_proces(l);
    } else {
        color_scheme_t cs = { .bg = color_palette[0], .fg = color_palette[l->color_idx] };
        // char * t =
        // if (label_process_ctx.uv_prev) {
        //     if (!ctx_check_val(label_process_ctx.uv_prev, label_process_ctx.uv, l->val_rep, l->val_offset_in_ctx)) {
        //         return;
        //     }
        // }

        if (l->type == LABEL_STATIC_TEXT) {
            dp("print static text : "); dp(l->text); dn();
            lcd_color_tptr_print(label_process_ctx.tp, l->text, cs, 0);
        } else {
            void * val_ptr = label_process_ctx.uv + l->val_offset_in_ctx;
            // if (val)
            // val_text_t vt = { .f = X10, .p = 1, .zl = 1, .zr = 1 };
            val_text_t * vt;
            if (l->vt_real) {
                vt = &l->vt;
            } else {
                vt = label_process_ctx.uv + l->vt_offset;
            }
            dp("val rep: "); dpd(l->val_rep.vs, 1); dp("   "); dpxd(val_ptr, 1, 4); dn();

            char * str = str_from_val(val_ptr, l->val_rep, *vt, l->len);
            lcd_color_tptr_print(label_process_ctx.tp, str, cs, l->len);
        }
    }
}

void widget_labels_proces(const widget_labels_t * l, tptr_t * tp, void * ctx, void * ctx_prev)
{
    color_palette = l->color_palette;
    label_process_ctx.tp = tp;
    label_process_ctx.uv = ctx;
    label_process_ctx.uv_prev = ctx_prev;

    dpn("process static labels");
    if (ctx_prev == 0) {
        list_process_all(l->static_labels, label_process);
    }

    dpn("process value labels");
    label_process(&l->label);
}




