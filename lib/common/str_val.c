#include <stdint.h>

/*
    либа для печати десятичного представления в текстовый буфер


*/

// total 32 bytes
unsigned str_val_buf_idx = 0;
char str_val_buf[2][14] = {};

char * str_val_buf_get(void)
{
    return str_val_buf[str_val_buf_idx];
}

void str_val_buf_lock(void)
{
    str_val_buf_idx = !str_val_buf_idx;
}

void dec_to_str_right_aligned(unsigned val, char * str, unsigned len, unsigned lzero)
{
    str[len] = 0;
    if (val == 0) {
        len--;
        str[len] = '0';
    }
    while (len) {
        len--;
        if (val) {
            str[len] = '0' + (val % 10);
            val /= 10;
        } else {
            if (lzero) {
                str[len] = '0';
            } else {
                str[len] = ' ';
            }
        }
    }
    if (val) {
        str[0] = '@';
    }
}

void hex_to_str(const void * val, char * str, unsigned size)
{
    static const char hex[] = "0123456789ABCDEF";
    str[size * 2] = 0;
    while (size) {
        unsigned x = *(uint8_t *)val;
        str[(size * 2) - 1] = hex[x & 0xF];
        x >>= 4;
        str[(size * 2) - 2] = hex[x & 0xF];
        val++;
        size--;
    }
}

void hex_dump_to_str(void * ptr, char * str, unsigned size, unsigned count)
{
    while (count--) {
        hex_to_str(ptr, str, size);
        ptr += size;
        str += size * 2;
        if (count) {
            *str = ' ';
            str++;
        }
    }
    *str = 0;
}
