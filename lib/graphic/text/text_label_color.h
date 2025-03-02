#include "color_scheme_type.h"
#include "text_field.h"

struct lvcolor {
    lcd_color_t color;
    label_t l;
};

void lp(const tf_ctx_t * tf, const label_t * l, color_scheme_t * cs, void * prev_ctx, void * ctx, unsigned idx);
