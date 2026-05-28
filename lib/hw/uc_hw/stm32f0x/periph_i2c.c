#include "periph_i2c.h"
#include "irq_vectors.h"


static const i2c_cfg_t * _cfg = 0;

static uint8_t _a = 0;
static const uint8_t * _tbuf = 0;
static uint8_t * _rbuf = 0;
static unsigned _tl = 0;
static unsigned _rl = 0;
static unsigned _status = I2C_STATUS_READY;
static unsigned _next_status = I2C_STATUS_READY;


static void i2c_handler(void)
{
    uint32_t isr = _cfg->i2c->ISR;

    if (isr & I2C_ISR_NACKF) {
        _cfg->i2c->ICR = I2C_ICR_NACKCF;
        _next_status = I2C_STATUS_NACK;
    }

    if (isr & I2C_ISR_TXIS) {
        if (_tl > 0) {
            _cfg->i2c->TXDR = *_tbuf++;
            _tl--;
        }
    }

    if (isr & I2C_ISR_RXNE) {
        if (_rl > 0) {
            *_rbuf++ = _cfg->i2c->RXDR;
            _rl--;
        }
    }

    if (isr & I2C_ISR_TC) {
        // tx-фаза завершилась (AUTOEND был 0) — repeated start для rx
        uint32_t cr2 = (uint32_t)_a << 1;            // SADD[7:1] — 7-bit адрес ведомого
        cr2 |= I2C_CR2_RD_WRN;                       // 1 = чтение
        cr2 |= (uint32_t)_rl << 16;                  // NBYTES[23:16] — байт в rx-фазе
        cr2 |= I2C_CR2_AUTOEND;                      // авто-STOP после NBYTES
        cr2 |= I2C_CR2_START;                        // запустить (repeated) START
        _cfg->i2c->CR2 = cr2;
    }

    if (isr & I2C_ISR_STOPF) {
        _cfg->i2c->ICR = I2C_ICR_STOPCF;
        _status = _next_status;
    }
}


void init_i2c(const i2c_cfg_t * cfg)
{
    _cfg = cfg;

    init_gpio_list(cfg->pins);

    if (cfg->i2c == I2C1) {
        if (cfg->clk_src == I2C_CLK_SYSCLK) {
            RCC->CFGR3 |= RCC_CFGR3_I2C1SW;
        } else {
            RCC->CFGR3 &= ~RCC_CFGR3_I2C1SW;
        }
    }

    pclk_ctrl(&cfg->pclk, 1);

    cfg->i2c->CR1 = 0;                               // disable PE: TIMINGR пишется только при PE=0

    cfg->i2c->TIMINGR = cfg->timing.raw;

    NVIC_SetHandler(cfg->irqn, i2c_handler);
    NVIC_EnableIRQ(cfg->irqn);

    cfg->i2c->CR1 |= I2C_CR1_TXIE;                   // TXIS: tx-регистр пуст, нужен новый байт
    cfg->i2c->CR1 |= I2C_CR1_RXIE;                   // RXNE: принят байт
    cfg->i2c->CR1 |= I2C_CR1_NACKIE;                 // NACKF: NACK от ведомого
    cfg->i2c->CR1 |= I2C_CR1_STOPIE;                 // STOPF: на шине обнаружен STOP
    cfg->i2c->CR1 |= I2C_CR1_TCIE;                   // TC: NBYTES передано, AUTOEND=0
    cfg->i2c->CR1 |= I2C_CR1_ERRIE;                  // ошибки: BERR/ARLO/OVR/PECERR/TIMEOUT
    cfg->i2c->CR1 |= I2C_CR1_PE;                     // peripheral enable
}


unsigned i2c_status(void)
{
    return _status;
}


void i2c_transaction(uint8_t addr, const void * tbuf, unsigned tlen, void * rbuf, unsigned rlen)
{
    if (_status == I2C_STATUS_BUSY) {
        return;
    }

    _next_status = I2C_STATUS_READY;
    _status = I2C_STATUS_BUSY;
    _a = addr;
    _tbuf = tbuf;
    _rbuf = rbuf;
    _tl = tlen;
    _rl = rlen;

    uint32_t cr2 = (uint32_t)addr << 1;              // SADD[7:1] — 7-bit адрес

    if (tlen > 0) {
        cr2 |= (uint32_t)tlen << 16;                 // NBYTES[23:16] — байт tx-фазы
        if (rlen == 0) {
            cr2 |= I2C_CR2_AUTOEND;                  // только запись — авто-STOP по NBYTES
        }
        // иначе AUTOEND=0: после tlen байт сработает TC, перейдём в rx через repeated start
    } else {
        cr2 |= (uint32_t)rlen << 16;                 // NBYTES — байт rx-фазы (0 для сканера)
        cr2 |= I2C_CR2_AUTOEND;                      // авто-STOP после NBYTES (или сразу после ACK при NBYTES=0)
        if (rlen > 0) {
            cr2 |= I2C_CR2_RD_WRN;                   // 1 = чтение
        }
    }

    cr2 |= I2C_CR2_START;                            // запустить START condition
    _cfg->i2c->CR2 = cr2;
}
