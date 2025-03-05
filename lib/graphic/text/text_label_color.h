#pragma once
#include "color_scheme_type.h"
#include "text_field.h"

typedef struct {
    lcd_color_t color;
    label_t l;
} lp_color_t;

void lp_color(const tf_ctx_t * tf, lcd_color_t bg_color, const label_list_t * list, unsigned idx, void * ctx, void * prev_ctx);

void lp(const tf_ctx_t * tf, const label_t * l, color_scheme_t * cs, void * prev_ctx, void * ctx, unsigned idx);
