#include <stdint.h>

unsigned str_cmp(const void * s1, const void * s2, unsigned len)
{
    for (unsigned i = 0; i < len; i++) {
        if(((uint8_t*)s1)[i] != ((uint8_t*)s2)[i]){
            return 0;
        }
    }
    return 1;
}

void str_cp(void * d, const void * s, unsigned len)
{
    for (unsigned i = 0; i < len; i++) {
        ((uint8_t*)d)[i] = ((uint8_t*)s)[i];
    }
}

unsigned str_find(const char * str, unsigned max_len, char c)
{
    unsigned n = 0;
    while (n < max_len) {
        if (str[n] == c) {
            break;
        }
        n++;
    }
    return n;
}

void str_fill(void * d, unsigned len, uint8_t c)
{
    for (unsigned i = 0; i < len; i++) {
        ((uint8_t*)d)[i] = c;
    }
}
