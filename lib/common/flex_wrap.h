#include <stdint.h>

/*
    обертка для статического выделения в BSS структур с flexible array member

    FLEX_WRAP(type, len) - возвращает указатель типа type на BSS память, выделенную под структуру и массив длиной len байт
*/

#define FLEX_WRAP(type, len) \
    (type *) & ( struct { \
        type list; \
        uint8_t data[len]; \
    } ) {}

#define FLEX_WRAP_ELEMENTS(type, element_type, len) FLEX_WRAP(type, sizeof(element_type) * len)
