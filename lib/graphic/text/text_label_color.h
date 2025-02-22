#include "color_scheme_type.h"
#include "text_field.h"

void label_static_print(const tf_ctx_t * tf, const label_static_t * l, color_scheme_t * cs, unsigned idx);

void label_value_print(const tf_ctx_t * tf, const label_value_t * l, color_scheme_t * cs, void * ctx, void * new_ctx);
