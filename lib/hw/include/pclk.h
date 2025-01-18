#pragma once

// описание тактирования периферии
enum plck_bus {
    PCLK_BUS_APB1 = 0,      // for rcc apb_div array indexes
    PCLK_BUS_APB2 = 1,
    PCLK_BUS_AHB,
    PCLK_BUS_IOP,
};

typedef struct {
    enum plck_bus bus;
    unsigned mask;
} pclk_t;

// управление тактированием периферии
void pclk_ctrl(const pclk_t * pclk, unsigned state);

// функции для получения частоты
unsigned pclk_f_hclk(void);                             // hclk - ядро
unsigned pclk_f(const pclk_t * pclk);                   // периферия
unsigned pclk_f_timer(const pclk_t * pclk);             // таймеры
