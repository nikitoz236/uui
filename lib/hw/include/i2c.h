#pragma once
#include <stdint.h>

struct i2c_cfg;

typedef struct i2c_cfg i2c_cfg_t;

typedef struct {
    void (*end)(unsigned status);
    const uint8_t * tx_buf;
    uint8_t * rx_buf;
    unsigned tx_len;
    unsigned rx_len;
    uint8_t dev_addr;
} i2c_transaction_t;


enum {
    I2C_STATUS_READY,
    I2C_STATUS_NACK,
    I2C_STATUS_BUSY,
};


void init_i2c(const i2c_cfg_t * cfg);

unsigned i2c_status(void);
// void i2c_transaction(uint8_t addr, const uint8_t * tbuf, unsigned tlen, uint8_t * rbuf, unsigned rlen);
void i2c_transaction(uint8_t addr, const void * tbuf, unsigned tlen, void * rbuf, unsigned rlen);
