#include "str_val_buf.h"

// total 32 bytes
unsigned str_val_buf_idx = 0;
char str_val_buf[2][STR_VAL_BUF_LEN] = {};

char * str_val_buf_get(void)
{
    char * str = str_val_buf[str_val_buf_idx];
    str_val_buf_idx = !str_val_buf_idx;
    return str;
}
