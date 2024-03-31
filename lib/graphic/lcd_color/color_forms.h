#include "api_lcd_color.h"
#include "forms.h"

static inline void form_fill(form_t * f, lcd_color_t color)
{
    lcd_rect(f->p.x, f->p.y, f->s.w, f->s.h, color);
}
