#pragma once
#include "color_scheme_type.h"
#include "text_field.h"

typedef struct {
    lcd_color_t color;
    label_t l;
} label_color_t;

void label_color(const tf_ctx_t * tf, const label_color_t * label, lcd_color_t bg_color, unsigned idx, void * ctx, void * prev_ctx);
void label_color_list(const tf_ctx_t * tf, const label_list_t * list, lcd_color_t bg_color, unsigned idx, void * ctx, void * prev_ctx);


void lp(const tf_ctx_t * tf, const label_t * l, color_scheme_t * cs, void * prev_ctx, void * ctx, unsigned idx);
void lcd_label_list(const tf_ctx_t * tf, const label_list_t * list, const color_scheme_t * cs, unsigned idx, void * ctx, void * prev_ctx);
