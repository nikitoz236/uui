#include <stdint.h>

void lcd_rect(unsigned x, unsigned y, unsigned w, unsigned h, unsigned color);
void lcd_bitmap(unsigned x, unsigned y, unsigned w, unsigned h, unsigned scale, void * buf);

void lcd_refresh(void);
