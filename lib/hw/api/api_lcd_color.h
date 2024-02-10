#include <stdint.h>
#include "color_type.h"

/*
    интерфейс для работы с цветным дисплеем

    функции для работы графики, которые нам должен обеспечить драйвер

    разделены на 2 ради оптимизации, одна заливает область цветом, другая изображением
    внутри могут быть как угодно оптимизированы, использовать dma итд
*/

void lcd_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned color);
void lcd_image(unsigned x, unsigned y, unsigned w, unsigned h, unsigned scale, lcd_color_t * buf);
