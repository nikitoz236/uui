
#define TEXT_LEN(str) (sizeof(str) - 1)

unsigned dec_to_str_right_aligned(unsigned val, char * str, unsigned len, unsigned lzero);
void hex_to_str(const void * val, char * str, unsigned size);
void hex_dump_to_str(const void * ptr, char * str, unsigned size, unsigned count);
