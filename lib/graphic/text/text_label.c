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

static lcd_color_t * _color_palette;
static unsigned _idx;
static label_process_ctx_t l_ctx;

void update_ctx(void * ctx, void * val, val_rep_t rep, const ctx_update_func_t * f)
{
    dp("  sublist ctx calc "); dpx((unsigned)ctx, 4); dp(" val ptr "); dpx((unsigned)val, 4); dp(" size "); dpd(rep.vs, 1); dp(" sign "); dpd(rep.s, 1); dn();

    if (rep.vs == VAL_SIZE_PTR) {
        f->p(ctx, val);
    } else {
        if (rep.s) {
            int v = val_ptr_to_signed(val, rep.vs);
            dp("    signed   "); dpd(v, 10); dn();
            f->s(ctx, v);
        } else {
            unsigned v = val_ptr_to_usnigned(val, rep.vs);
            dp("    unsigned "); dpd(v, 10); dn();
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

static void label_process(const label_t * l);

void label_sublist_proces(const label_t * l)
{
    const label_sublist_t * sl = &l->sublist;
    void * val_ptr = l_ctx.uv + l->val_offset_in_ctx;

    label_process_ctx_t stored = l_ctx;
    unsigned sub_ctx_offset = l->sub_list_ctx_offset;

    l_ctx.uv += sub_ctx_offset;
    if (l_ctx.uv_prev) {
        l_ctx.uv_prev += sub_ctx_offset;
    }

    update_ctx(l_ctx.uv, val_ptr, l->val_rep, &sl->update_func);

    tptr_t sub_tp = text_ptr_export(l_ctx.tp, (xy_t){});
    l_ctx.tp = &sub_tp;

    if (l_ctx.uv_prev == 0) {
        list_process_all(sl->labels_static, label_process);
    }

    list_process_all(sl->labels_dynamic, label_process);

    l_ctx = stored;
}

// нам нужно еще относительное положение
static void label_process(const label_t * l)
{
    // dp("process label "); dpx((unsigned)l, 4); dp(" : "); dpxd(l, 1, sizeof(label_t)); dn();
    dp("xy "); dpd(l->pos.x, 3); dp(","); dpd(l->pos.y, 3); dp(" type "); dpd(l->type, 1); dn();
    text_ptr_set_char_pos(l_ctx.tp, l->pos);
    if (l->type == LABEL_SUB_LIST) {
        label_sublist_proces(l);
    } else {
        color_scheme_t cs = { .bg = _color_palette[0], .fg = _color_palette[l->color_idx] };

        if (l_ctx.uv_prev) {
            if (!ctx_check_val(l_ctx.uv_prev, l_ctx.uv, l->val_rep, l->val_offset_in_ctx)) {
                dpn("       skip");
                return;
            }
        }

        if (l->type == LABEL_STATIC_TEXT) {
            // dp("print static text : "); dp(l->text); dn();
            lcd_color_tptr_print(l_ctx.tp, l->text, cs, 0);
        } else {
            void * val_ptr = l_ctx.uv + l->val_offset_in_ctx;
            const val_text_t * vt;
            if (l->vt_ctx) {
                // vt = (val_text_t *)(l_ctx.uv + l->vt_offset);
                vt = l_ctx.uv + l->vt_offset;
            } else {
                vt = &l->vt;
            }
            dp("    val rep: "); dpd(l->val_rep.vs, 1); dp("   "); dpxd(val_ptr, 1, 4); dn();

            const char * str = str_from_val(val_ptr, l->val_rep, *vt, l->len);
            lcd_color_tptr_print(l_ctx.tp, str, cs, l->len);
        }
    }
}

void widget_labels_proces(const widget_labels_t * wl, tptr_t * tp, void * ctx, void * ctx_prev)
{
    _color_palette = wl->color_palette;
    // _idx = idx;
    l_ctx.tp = tp;
    l_ctx.uv = ctx;
    l_ctx.uv_prev = ctx_prev;

    // dpn("process value labels");
    label_process(&wl->label);
}
