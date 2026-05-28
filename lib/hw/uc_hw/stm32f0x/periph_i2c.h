#pragma once

#include "i2c.h"
#include "gpio.h"
#include "pclk.h"
#include "periph_header.h"

typedef enum {
    I2C_CLK_HSI,
    I2C_CLK_SYSCLK,
} i2c_clk_src_t;

// TIMINGR register layout (RM0091 §27.7.5)
typedef union {
    uint32_t raw;
    struct __attribute__((packed)) {
        uint8_t scll;          // [7:0]   SCL low period в тиках presc
        uint8_t sclh;          // [15:8]  SCL high period в тиках presc
        uint8_t sdadel : 4;    // [19:16] data hold time = SDADEL * t_presc
        uint8_t scldel : 4;    // [23:20] data setup time = (SCLDEL+1) * t_presc
        uint8_t        : 4;    // [27:24] reserved
        uint8_t presc  : 4;    // [31:28] f_presc = f_in / (PRESC+1)
    };
} i2c_timingr_t;

struct i2c_cfg {
    gpio_list_t * pins;
    I2C_TypeDef * i2c;
    uint8_t irqn;
    pclk_t pclk;
    i2c_clk_src_t clk_src;
    i2c_timingr_t timing;
};

// Расчёт TIMINGR препроцессором.
// f_in — входная частота I2C-блока (выбирается clk_src + RCC).
// Целимся в t_presc = 125 нс (f_presc = 8 МГц).
// Ограничения:
//   - f_in >= 8 МГц и кратен 8 (8/16/24/32/40/48 МГц)
//   - только Sm (100 кГц) и Fm (400 кГц), Fm+ (1 МГц) не поддержан

#define _I2C_PRESC(f_in)       ((f_in) / 8000000 - 1)
#define _I2C_TICKS(f_in, f_t)  ((f_in) / (((f_in) / 8000000) * (f_t)))

// Sm 100 кГц. Для f_in = 8 МГц: presc=0, scldel=4, sdadel=2, sclh=35, scll=43.
#define I2C_TIMING_100K(f_in) \
    .timing = { \
        .presc = _I2C_PRESC(f_in), \
        .scldel = 4, \
        .sdadel = 2, \
        .sclh = _I2C_TICKS(f_in, 100000) - _I2C_TICKS(f_in, 100000) * 11 / 20 - 1, \
        .scll = _I2C_TICKS(f_in, 100000) * 11 / 20 - 1, \
    }

// Fm 400 кГц. Для f_in = 8 МГц: presc=0, scldel=1, sdadel=0, sclh=6, scll=12.
#define I2C_TIMING_400K(f_in) \
    .timing = { \
        .presc = _I2C_PRESC(f_in), \
        .scldel = 1, \
        .sdadel = 0, \
        .sclh = _I2C_TICKS(f_in, 400000) - _I2C_TICKS(f_in, 400000) * 2 / 3 - 1, \
        .scll = _I2C_TICKS(f_in, 400000) * 2 / 3 - 1, \
    }
