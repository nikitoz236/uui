#include <stdint.h>
#include "color_type.h"

void lcd_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned color);
void lcd_image(unsigned x, unsigned y, unsigned w, unsigned h, unsigned scale, lcd_color_t * buf);
