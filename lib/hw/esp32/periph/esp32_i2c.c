#include "esp32_i2c.h"

#define DP_NAME "i2c"
#define DP_OFF
#include "dp.h"

const i2c_cfg_t * _cfg = 0;

typedef union {
    uint32_t val;
    struct {
        uint8_t byte_n;
        uint8_t ack_check_en : 1;
        uint8_t ack_exp : 1;
        uint8_t ack_value : 1;
        enum {
            I2C_OP_WRITE = 1,
            I2C_OP_STOP = 2,
            I2C_OP_READ = 3,
            I2C_OP_END = 4,
            I2C_OP_RSTART = 6,
        } op_code : 3;
    };
} i2c_cmd_t;

static void calc_bus_clk(i2c_dev_t * dev, unsigned freq)
{
    const uint32_t source_clk = 40000000;
    unsigned clkm_div = source_clk / (freq * 1024) + 1;     // = 1
    unsigned sclk_freq = source_clk / clkm_div;             // = 40 000 000
    unsigned half_cycle = sclk_freq / freq / 2;             // = 50

    // default, scl_wait_high < scl_high
    // Make 80KHz as a boundary here, because when working at lower frequency, too much scl_wait_high will faster the frequency
    // according to some hardware behaviors.

    unsigned scl_wait_high = (freq >= 80 * 1000) ? (half_cycle / 2 - 2) : (half_cycle / 4);
    unsigned scl_high = half_cycle - scl_wait_high;
    unsigned sda_sample = half_cycle / 2;

    // ASSERT((scl_wait_high < sda_sample) && (sda_sample < scl_high));

    unsigned sda_hold = half_cycle / 4;

    //SCL

    /* According to the Technical Reference Manual, the following timings must be subtracted by 1.
     * However, according to the practical measurement and some hardware behaviour, if wait_high_period and scl_high minus one.
     * The SCL frequency would be a little higher than expected. Therefore, the solution
     * here is not to minus scl_high as well as scl_wait high, and the frequency will be absolutely accurate to all frequency
     * to some extent. */

    i2c_clk_conf_reg_t tmp;
    tmp.val = dev->clk_conf.val;
    tmp.sclk_div_num = clkm_div - 1;
    dev->clk_conf.val = tmp.val;

    dev->scl_low_period.scl_low_period = half_cycle - 1;
    dev->scl_high_period.scl_high_period = scl_high;
    dev->scl_high_period.scl_wait_high_period = scl_wait_high;

    //sda sample
    dev->sda_hold.sda_hold_time = sda_hold - 1;
    dev->sda_sample.sda_sample_time = sda_sample - 1;
    //setup
    dev->scl_rstart_setup.scl_rstart_setup_time = half_cycle - 1;
    dev->scl_stop_setup.scl_stop_setup_time = half_cycle - 1;
    //hold
    dev->scl_start_hold.scl_start_hold_time = half_cycle - 1;
    dev->scl_stop_hold.scl_stop_hold_time = half_cycle - 1;

    //default we set the timeout value to about 10 bus cycles
    // log(20*half_cycle)/log(2) = log(half_cycle)/log(2) +  log(20)/log(2)
    unsigned tout = (int)(sizeof(half_cycle) * 8 - __builtin_clz(5 * half_cycle)) + 2;

    dev->to.time_out_value = tout;
    dev->to.time_out_en = 1;
}

void init_i2c(const i2c_cfg_t * cfg)
{
    _cfg = cfg;

    init_gpio_list(_cfg->pin_list);

    pclk_ctrl(_cfg->i2c_pclk, 1);

    calc_bus_clk(_cfg->dev, _cfg->freq);

    _cfg->dev->ctr.conf_upgate = 1;

    _cfg->dev->ctr.ms_mode = 1;
    _cfg->dev->ctr.rx_full_ack_level = 0;
    _cfg->dev->ctr.arbitration_en = 0;
    _cfg->dev->ctr.scl_force_out = 0;
    _cfg->dev->ctr.sda_force_out = 0;

    _cfg->dev->filter_cfg.scl_filter_thres = 7;
    _cfg->dev->filter_cfg.sda_filter_thres = 7;

    _cfg->dev->ctr.conf_upgate = 1;
}

static void tx_data_to_fifo(i2c_dev_t * dev, const void * ptr, unsigned len)
{
    uint8_t * p = (uint8_t *)ptr;
    while (len--) {
        dev->data.val = *p++;
    }
}

static void rx_data_from_fifo(i2c_dev_t * dev, void * ptr, unsigned len)
{
    uint8_t * p = (uint8_t *)ptr;
    while (len--) {
        *p++ = dev->data.val;
    }
}

#include "delay_blocking.h"

void i2c_transaction(uint8_t addr, const void * tbuf, unsigned tlen, void * rbuf, unsigned rlen)
{
    _cfg->dev->int_clr.trans_complete_int_clr = 1;
    _cfg->dev->int_clr.nack_int_clr = 1;

    _cfg->dev->fifo_conf.rx_fifo_rst = 1;
    _cfg->dev->fifo_conf.tx_fifo_rst = 1;

    _cfg->dev->fifo_conf.rx_fifo_rst = 0;
    _cfg->dev->fifo_conf.tx_fifo_rst = 0;

    unsigned op_idx = 0;
    _cfg->dev->comd[op_idx++].val = (i2c_cmd_t){ .op_code = I2C_OP_RSTART }.val;
    _cfg->dev->comd[op_idx++].val = (i2c_cmd_t){ .op_code = I2C_OP_WRITE, .byte_n = 1, .ack_check_en = 1 }.val;

    unsigned addr_dir = addr << 1;

    if (rlen) {
        if (tlen == 0) {
            addr_dir |= 1;
        }
    }
    _cfg->dev->data.val = addr_dir;

    if (tlen) {
        unsigned tx_byte_once = tlen;
        // while (tlen < 31)
        // if (tlen > 31) {
        //     tx_byte_once = 31;
        // }
        _cfg->dev->comd[op_idx++].val = (i2c_cmd_t){ .op_code = I2C_OP_WRITE, .byte_n = tx_byte_once }.val;
        tx_data_to_fifo(_cfg->dev, tbuf, tx_byte_once);

        // if (tx_byte_once == 31) {
        //     _cfg->dev->comd[op_idx++].val = (i2c_cmd_t){ .op_code = I2C_OP_END }.val;
        // }
    }
    if (rlen) {
        if (tlen) {
            addr_dir |= 1;
            _cfg->dev->data.val = addr_dir;
            _cfg->dev->comd[op_idx++].val = (i2c_cmd_t){ .op_code = I2C_OP_RSTART }.val;
            _cfg->dev->comd[op_idx++].val = (i2c_cmd_t){ .op_code = I2C_OP_WRITE, .byte_n = 1, .ack_check_en = 1 }.val;
        }
        _cfg->dev->comd[op_idx++].val = (i2c_cmd_t){ .op_code = I2C_OP_READ, .byte_n = rlen - 1, .ack_value = 0 }.val;
        _cfg->dev->comd[op_idx++].val = (i2c_cmd_t){ .op_code = I2C_OP_READ, .byte_n = 1, .ack_value = 1 }.val;
    }

    _cfg->dev->comd[op_idx++].val = (i2c_cmd_t){ .op_code = I2C_OP_STOP }.val;

    // dp("before run sr: "); dpx(_cfg->dev->sr.val, 4); dp(" int: "); dpx(_cfg->dev->int_raw.val, 4); dp(" cmd: ");
    // for (unsigned i = 0; i < 8; i++) {
    //     uint32_t cmd = _cfg->dev->comd[i].val;
    //     dpx(cmd, 4); dp(" ");
    // }
    // dn();

    _cfg->dev->ctr.conf_upgate = 1;
    _cfg->dev->ctr.trans_start = 1;

    // delay_us(500);

    // dp("after run  sr: "); dpx(_cfg->dev->sr.val, 4); dp(" int: "); dpx(_cfg->dev->int_raw.val, 4); dp(" cmd: ");
    // for (unsigned i = 0; i < 8; i++) {
    //     uint32_t cmd = _cfg->dev->comd[i].val;
    //     dpx(cmd, 4); dp(" ");
    // }
    // dn();

    if (rlen) {
        while (i2c_status() == I2C_STATUS_BUSY) {};
        if (i2c_status() == I2C_STATUS_READY) {
            rx_data_from_fifo(_cfg->dev, rbuf, rlen);
        }
    }
}

unsigned i2c_status(void)
{
    if (_cfg->dev->int_raw.trans_complete_int_raw == 0) {
        return I2C_STATUS_BUSY;
    }
    if (I2C0.int_raw.nack_int_raw) {
        return I2C_STATUS_NACK;
    }
    return I2C_STATUS_READY;
}
