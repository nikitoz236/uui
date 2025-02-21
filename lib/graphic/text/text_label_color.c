#include "str_val_buf.h"
#include "text_label_color.h"
#include "lcd_text_color.h"

// #include <stdio.h>

void label_static_print(const tf_ctx_t * tf, const label_static_t * l, color_scheme_t * cs, unsigned idx)
{
    const char * str;
    if (l->to_str) {
        str = l->to_str(idx);
    } else {
        str = l->text;
    }
    lcd_color_text_raw_print(str, tf->tfcfg->fcfg, cs, &tf->pos, &tf->tfcfg->limit_char, &l->pos_char, l->len);
}

void label_value_print(const tf_ctx_t * tf, const label_value_t * l, color_scheme_t * cs, void * ctx)
{
    const char * str;

    if (l->to_str) {
        str = l->to_str(val_unpack(ctx + l->offset, l->rep, 0));
    } else {
        str = str_val_buf_get();
        val_text_ptr_to_str(str, ctx + l->offset, l->rep, l->vt, l->len);
    }
    lcd_color_text_raw_print(str, tf->tfcfg->fcfg, cs, &tf->pos, &tf->tfcfg->limit_char, &l->pos_char, 0);
}

// void label_value_update(const tf_ctx_t * tf, const label_value_t * l, unsigned active, void * ctx, void * ctx_actual)
// {

// }
