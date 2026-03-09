#pragma once
#include "text_field.h"
#include "lcd_text_color.h"

static inline void lcd_color_tf_print(const char * str, const tf_ctx_t * tf_ctx, const color_scheme_t * cs, xy_t * char_xy, unsigned len)
{
    lcd_color_text_raw_print(str, tf_ctx->tfcfg->fcfg, cs, &tf_ctx->xy, &tf_ctx->size, char_xy, len);
}

