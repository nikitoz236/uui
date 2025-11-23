#include "periph_i2c.h"
#include "irq_vectors.h"

#define CR1             CTL0
#define CR2             CTL1
#define SR1             STAT0
#define SR2             STAT1
#define TRISE           RT
#define CCR             CKCFG
#define DR              DATA

#define I2C_CR1_START   I2C_CTL0_START
#define I2C_CR1_PE      I2C_CTL0_I2CEN
#define I2C_CR1_STOP    I2C_CTL0_STOP
#define I2C_CR1_ACK     I2C_CTL0_ACKEN

#define I2C_CR2_ITBUFEN I2C_CTL1_BUFIE
#define I2C_CR2_ITERREN I2C_CTL1_ERRIE
#define I2C_CR2_ITEVTEN I2C_CTL1_EVIE
#define I2C_CR2_FREQ_0  1

#define I2C_SR1_AF      I2C_STAT0_AERR
#define I2C_SR1_SB      I2C_STAT0_SBSEND
#define I2C_SR1_ADDR    I2C_STAT0_ADDSEND
#define I2C_SR1_TXE     I2C_STAT0_TBE
#define I2C_SR1_RXNE    I2C_STAT0_RBNE

#define I2C_CCR_FS      I2C_CKCFG_FAST

// void dc(char c);




static const i2c_cfg_t * _cfg = 0;

static uint8_t _a = 0;
static const uint8_t * _tbuf = 0;
static uint8_t * _rbuf = 0;
static unsigned _tl = 0;
static unsigned _rl = 0;
static unsigned _status = I2C_STATUS_READY;
static void (*_end_cb)(unsigned status) = 0;

static void notify(unsigned status)
{
    _status = status;
    if (_end_cb) {
        _end_cb(status);
    }
}

static void i2c_err_handler(void)
{
    // dc(0xF2);

    // Acknowledge failure
    if (_cfg->i2c->SR1 & I2C_SR1_AF) {
        _cfg->i2c->SR1 = ~I2C_SR1_AF;
        notify(I2C_STATUS_NACK);
        _cfg->i2c->CR1 |= I2C_CR1_STOP;
    }
}

static void i2c_handler(void)
{
    // dc(0xF1);

    // Start condition generated
    if (_cfg->i2c->SR1 & I2C_SR1_SB) {
        if (_tl == 0) {
            // restart
            if (_rl) {
                _a |= 1;
                if (_rl > 1) {
                    _cfg->i2c->CR1 |= I2C_CR1_ACK;
                } else {
                    _cfg->i2c->CR1 &= ~I2C_CR1_ACK;
                }
            }
        }
        _cfg->i2c->DR = _a;
    }

    // Address sent
    if (_cfg->i2c->SR1 & I2C_SR1_ADDR) {
        (void)_cfg->i2c->SR2;
        if (_tl == 0) {
            if (_rl <= 1) {
                _cfg->i2c->CR1 |= I2C_CR1_STOP;
            }
        }
    }

    // tx reg empty
    if (_cfg->i2c->SR1 & I2C_SR1_TXE) {
        if (_tl == 0) {
            if (_rl) {
                _cfg->i2c->CR1 |= I2C_CR1_START;
            } else {
                _cfg->i2c->CR1 |= I2C_CR1_STOP;
                notify(I2C_STATUS_READY);
            }
        } else {
            _tl--;
            _cfg->i2c->DR = *_tbuf++;
        }
    }

    // data recieved
    if (_cfg->i2c->SR1 & I2C_SR1_RXNE) {
        _rl--;
        if (_rl == 1) {
            _cfg->i2c->CR1 &= ~I2C_CR1_ACK;
            _cfg->i2c->CR1 |= I2C_CR1_STOP;
        }
        *_rbuf++ = _cfg->i2c->DR;
        if (_rl == 0) {
            notify(I2C_STATUS_READY);
        }
    }
}

void init_i2c(const i2c_cfg_t * cfg)
{
    _cfg = cfg;
    init_gpio_list(cfg->pins);
    pclk_ctrl(&cfg->pclk, 1);

    unsigned periph_f = pclk_f(&cfg->pclk);

    cfg->i2c->CR1 &= ~I2C_CR1_PE;

    cfg->i2c->CR2 |= I2C_CR2_ITBUFEN;
    cfg->i2c->CR2 |= I2C_CR2_ITERREN;
    cfg->i2c->CR2 |= I2C_CR2_ITEVTEN;

    cfg->i2c->CR2 |= I2C_CR2_FREQ_0 * (periph_f / 1000000);

    cfg->i2c->TRISE = ((periph_f / (4 * cfg->freq)) + 1);
    cfg->i2c->CCR = (periph_f / (2 * cfg->freq));

    if (cfg->freq > 100000) {
        cfg->i2c->CCR |= I2C_CCR_FS;
    }

    NVIC_SetHandler(cfg->irqn, i2c_handler);
    NVIC_EnableIRQ(cfg->irqn);

    NVIC_SetHandler(cfg->err_irqn, i2c_err_handler);
    NVIC_EnableIRQ(cfg->err_irqn);

    cfg->i2c->CR1 |= I2C_CR1_PE;
}

unsigned i2c_status(void)
{
    return _status;
}

void i2c_transaction(uint8_t addr, const uint8_t * tbuf, unsigned tlen, uint8_t * rbuf, unsigned rlen)
{
    if (_status == I2C_STATUS_BUSY) {
        return;
    }

    _status = I2C_STATUS_BUSY;
    _tbuf = tbuf;
    _rbuf = rbuf;
    _tl = tlen;
    _rl = rlen;
    _a = addr << 1;

    _cfg->i2c->CR1 |= I2C_CR1_START;
}
