#include "api_lcd_color.h"
#include "forms.h"
#include "color_type.h"

static inline void draw_color_form(form_t * f, lcd_color_t color)
{
    lcd_rect(f->p.x, f->p.y, f->s.w, f->s.h, color);
};
