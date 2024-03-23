/*
    либа для печати десятичного представления в текстовый буфер


*/

void dec_to_str_right_aligned(unsigned val, char * str, unsigned len, unsigned lzero)
{
    str[len] = 0;
    if (val == 0) {
        str[len - 1] = '0';
        return;
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
