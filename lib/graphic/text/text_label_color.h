#include "color_scheme_type.h"
#include "text_field.h"

struct lscolor {
    lcd_color_t color;
    label_static_t l;
};

struct lvcolor {
    lcd_color_t color;
    label_t l;
};

void label_static_print(const tf_ctx_t * tf, const label_static_t * l, color_scheme_t * cs, unsigned idx);

#define label_value_print lp

void lp(const tf_ctx_t * tf, const label_t * l, color_scheme_t * cs, void * prev_ctx, void * ctx, unsigned idx);
