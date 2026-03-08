#include "api_lcd_color.h"
#include "forms.h"
#include "color_type.h"
#include "forms_split.h"

static inline void draw_color_form(form_t * f, lcd_color_t color)
{
    lcd_rect(f->p.x, f->p.y, f->s.w, f->s.h, color);
};

static inline void draw_frame(form_t * f, unsigned width, lcd_color_t color)
{
    form_t line;
    for (dimension_t d = 0; d < DIMENSION_COUNT; d++) {
        for (form_edge_t e = 0; e < EDGE_COUNT; e++) {
            line = *f;
            form_reduce(&line, d, e, width);
            draw_color_form(&line, color);
        }
    }
}
