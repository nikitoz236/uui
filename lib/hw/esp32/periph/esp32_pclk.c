#include "esp32_pclk.h"
#include "soc/system_struct.h"

void pclk_ctrl(pclk_t pclk, unsigned state)
{
    uint32_t mask = 1 << (pclk & 31);
    volatile uint32_t * clk_en_reg;
    if (pclk < 32) {
        clk_en_reg = &SYSTEM.perip_clk_en0.val;
    } else {
        clk_en_reg = &SYSTEM.perip_clk_en1.val;
    }
    if (state) {
        *clk_en_reg |= mask;
    } else {
        *clk_en_reg &= ~mask;
    }
}

void pclk_reset(pclk_t pclk)
{
    uint32_t mask = 1 << (pclk & 31);
    volatile uint32_t * rst_en_reg;
    if (pclk < 32) {
        rst_en_reg = &SYSTEM.perip_rst_en0.val;
    } else {
        rst_en_reg = &SYSTEM.perip_rst_en1.val;
    }
    *rst_en_reg |= mask;
    *rst_en_reg &= ~mask;
}
