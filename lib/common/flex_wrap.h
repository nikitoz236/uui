#include <stdint.h>

/*
    обертка для статического выделения в BSS структур с flexible array member

    FLEX_WRAP(type, size) - возвращает указатель типа type на BSS память, выделенную под структуру и массив длиной size байт
*/

#define FLEX_WRAP(type, size) \
    (type *) & ( struct { \
        type list; \
        uint8_t data[size]; \
    } ) {}
