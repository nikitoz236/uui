
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
