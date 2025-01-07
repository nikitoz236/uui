#include <stdio.h>
#include "color_type.h"

// #define C1      COLOR(0x4d3143)

const lcd_color_t c2 = COLOR(0x4d3143);
// const lcd_color_t c2 = (lcd_color_t)((__color_rgb_565_t){ .r = 10, b = 50 });

int main()
{
    unsigned rgb = 0x4d3143;
    // lcd_color_t c = ((__color_rgb_565_t){ .r = 2, .g = 5, }).val;
    // lcd_color_t c = ((__color_rgb_565_t){ .r = 2, .g = 5, }).val;

    // lcd_color_t c = C1;

    printf("color convert test RGB %06X, LCD %04X\n", rgb, c2);


//  0b 01001101  00110001  01000011
//  0b 01001 001100 01000

    return 0;
}
