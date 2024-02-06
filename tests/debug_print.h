#include <stdio.h>

#define debug_print(format, ...) \
    printf(format, ##__VA_ARGS__)
