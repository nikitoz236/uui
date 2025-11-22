#include <stdio.h>
#include <stdint.h>

struct rgb {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct rgb rgb_buf[2];

int main()
{
    printf("rgp array size test\n");

    printf("sizeof(rgb_buf) = %zu\n", sizeof(rgb_buf));

    return 0;
}
