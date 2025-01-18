#pragma once
// интерфейс для работы с модулем rcc
// общий конфиг и управление тактированием периферии

typedef enum {
    HCLK_DIV1 = 0,
    HCLK_DIV2 = 8,
    HCLK_DIV4 = 9,
    HCLK_DIV8 = 10,
    HCLK_DIV16 = 11,
    HCLK_DIV64 = 12,
    HCLK_DIV128 = 13,
    HCLK_DIV256 = 14,
    HCLK_DIV512 = 15,
} hclk_div_t;

typedef enum {
    APB_DIV1 = 0,
    APB_DIV2 = 4,
    APB_DIV4 = 5,
    APB_DIV8 = 6,
    APB_DIV16 = 7,
} apb_div_t;

// структура с общим конфигом
struct hw_rcc_cfg;
typedef struct hw_rcc_cfg hw_rcc_cfg_t;

// применение конфига блока rcc
void rcc_apply_cfg(const hw_rcc_cfg_t * cfg);
