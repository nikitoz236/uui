#include "xl9555.h"
#include "i2c.h"
#include "buf_endian.h"

#define XL9555_ADDR(gpio) (XL9555_BASE_ADDR | (gpio)->addr_offset)

enum {
    XL9555_REG_INPUT    = 0,
    XL9555_REG_OUTPUT   = 1,
    XL9555_REG_POLARITY = 2,
    XL9555_REG_CONFIG   = 3,
};

static uint16_t read_reg(uint8_t chip_addr, uint8_t reg)
{
    uint16_t val = 0;
    uint8_t cmd = reg << 1;
    i2c_transaction(chip_addr, &cmd, 1, &val, 2);
    while (i2c_status() == I2C_STATUS_BUSY) {};
    return val;
}

static void write_reg(uint8_t chip_addr, uint8_t reg, uint16_t val)
{
    uint8_t data[3] = { [0] = reg << 1 };
    u16_to_le_buf8(&data[1], val);
    i2c_transaction(chip_addr, data, 3, 0, 0);
    while (i2c_status() == I2C_STATUS_BUSY) {};
}

void init_xl9555_gpio(const xl9555_gpio_t * gpio)
{
    uint8_t chip_addr = XL9555_ADDR(gpio);
    uint16_t mask = 1 << gpio->pin;

    uint16_t pol = read_reg(chip_addr, XL9555_REG_POLARITY);
    if (gpio->polarity) {
        pol |= mask;
    } else {
        pol &= ~mask;
    }
    write_reg(chip_addr, XL9555_REG_POLARITY, pol);

    uint16_t cfg = read_reg(chip_addr, XL9555_REG_CONFIG);
    if (gpio->dir) {
        cfg |= mask;
    } else {
        cfg &= ~mask;
    }
    write_reg(chip_addr, XL9555_REG_CONFIG, cfg);
}

void xl9555_gpio_set(const xl9555_gpio_t * gpio, unsigned state)
{
    uint8_t chip_addr = XL9555_ADDR(gpio);
    uint16_t mask = 1 << gpio->pin;
    uint16_t out = read_reg(chip_addr, XL9555_REG_OUTPUT);
    if (state) {
        out |= mask;
    } else {
        out &= ~mask;
    }
    write_reg(chip_addr, XL9555_REG_OUTPUT, out);
}

unsigned xl9555_gpio_get(const xl9555_gpio_t * gpio)
{
    uint8_t chip_addr = XL9555_ADDR(gpio);
    uint16_t mask = 1 << gpio->pin;
    uint16_t in = read_reg(chip_addr, XL9555_REG_INPUT);
    if (in & mask) {
        return 1;
    }
    return 0;
}
