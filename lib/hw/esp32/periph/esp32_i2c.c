#include "esp32_i2c.h"

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


void calc_bus_clk(void)
{
    uint32_t source_clk = 40000000;
    uint32_t bus_freq = 400000;

    i2c_dev_t * hw = _cfg->dev;

    struct clk_vals {
        uint16_t clkm_div;          /*!< I2C core clock divider */
        uint16_t scl_low;           /*!< I2C scl low period */
        uint16_t scl_high;          /*!< I2C scl high period */
        uint16_t scl_wait_high;     /*!< I2C scl wait_high period */
        uint16_t sda_hold;          /*!< I2C scl low period */
        uint16_t sda_sample;        /*!< I2C sda sample time */
        uint16_t setup;             /*!< I2C start and stop condition setup period */
        uint16_t hold;              /*!< I2C start and stop condition hold period  */
        uint16_t tout;              /*!< I2C bus timeout period */
    } clk_vals;

    struct clk_vals * clk_cal = &clk_vals;

    uint32_t clkm_div = source_clk / (bus_freq * 1024) + 1;     // = 1
    uint32_t sclk_freq = source_clk / clkm_div;                 // = 40 000 000
    uint32_t half_cycle = sclk_freq / bus_freq / 2;             // = 50

    // dp("clkm_div : "); dpd(clkm_div, 5); dn();

    //SCL
    clk_cal->clkm_div = clkm_div;
    clk_cal->scl_low = half_cycle;
    // default, scl_wait_high < scl_high
    // Make 80KHz as a boundary here, because when working at lower frequency, too much scl_wait_high will faster the frequency
    // according to some hardware behaviors.

    clk_cal->scl_wait_high = (bus_freq >= 80 * 1000) ? (half_cycle / 2 - 2) : (half_cycle / 4);
    clk_cal->scl_high = half_cycle - clk_cal->scl_wait_high;
    clk_cal->sda_hold = half_cycle / 4;
    clk_cal->sda_sample = half_cycle / 2;
    clk_cal->setup = half_cycle;
    clk_cal->hold = half_cycle;

    //default we set the timeout value to about 10 bus cycles
    // log(20*half_cycle)/log(2) = log(half_cycle)/log(2) +  log(20)/log(2)
    clk_cal->tout = (int)(sizeof(half_cycle) * 8 - __builtin_clz(5 * half_cycle)) + 2;

    /* Verify the assumptions made by the hardware */

    // HAL_ASSERT(clk_cal->scl_wait_high < clk_cal->sda_sample &&
    //            clk_cal->sda_sample < clk_cal->scl_high);

    struct clk_vals * bus_cfg = &clk_vals;


    // HAL_FORCE_MODIFY_U32_REG_FIELD(hw->clk_conf, sclk_div_num, );

    /* According to the Technical Reference Manual, the following timings must be subtracted by 1.
     * However, according to the practical measurement and some hardware behaviour, if wait_high_period and scl_high minus one.
     * The SCL frequency would be a little higher than expected. Therefore, the solution
     * here is not to minus scl_high as well as scl_wait high, and the frequency will be absolutely accurate to all frequency
     * to some extent. */

    i2c_clk_conf_reg_t tmp;
    tmp.val = hw->clk_conf.val;
    tmp.sclk_div_num = bus_cfg->clkm_div - 1;
    hw->clk_conf.val = tmp.val;

    hw->scl_low_period.scl_low_period = bus_cfg->scl_low - 1;
    hw->scl_high_period.scl_high_period = bus_cfg->scl_high;
    hw->scl_high_period.scl_wait_high_period = bus_cfg->scl_wait_high;
    //sda sample
    hw->sda_hold.sda_hold_time = bus_cfg->sda_hold - 1;
    hw->sda_sample.sda_sample_time = bus_cfg->sda_sample - 1;
    //setup
    hw->scl_rstart_setup.scl_rstart_setup_time = bus_cfg->setup - 1;
    hw->scl_stop_setup.scl_stop_setup_time = bus_cfg->setup - 1;
    //hold
    hw->scl_start_hold.scl_start_hold_time = bus_cfg->hold - 1;
    hw->scl_stop_hold.scl_stop_hold_time = bus_cfg->hold - 1;
    hw->to.time_out_value = bus_cfg->tout;
    hw->to.time_out_en = 1;
}

void init_i2c(const i2c_cfg_t * cfg)
{
    _cfg = cfg;

    init_gpio_list(_cfg->pin_list);

    pclk_ctrl(_cfg->i2c_pclk, 1);

    calc_bus_clk();

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

void i2c_transaction(uint8_t addr, const uint8_t * tbuf, unsigned tlen, uint8_t * rbuf, unsigned rlen)
{
    _cfg->dev->int_clr.trans_complete_int_clr = 1;
    _cfg->dev->int_clr.nack_int_clr = 1;

    unsigned op_idx = 0;
    _cfg->dev->comd[op_idx++].val = (i2c_cmd_t){ .op_code = I2C_OP_RSTART }.val;

    _cfg->dev->comd[1].val = (i2c_cmd_t){ .op_code = I2C_OP_WRITE, .byte_n = 1, .ack_value = 0, .ack_exp = 0, .ack_check_en = 1 }.val;
    _cfg->dev->comd[2].val = (i2c_cmd_t){ .op_code = I2C_OP_STOP }.val;

    // unsigned tx_byte_n = 1 + tlen;

    _cfg->dev->fifo_conf.rx_fifo_rst = 1;
    _cfg->dev->fifo_conf.tx_fifo_rst = 1;

    _cfg->dev->fifo_conf.rx_fifo_rst = 0;
    _cfg->dev->fifo_conf.tx_fifo_rst = 0;

    unsigned addr_dir = addr << 1;

    if (tlen == 0) {
        addr |= 1;
    }

    _cfg->dev->data.val = addr_dir;
    _cfg->dev->ctr.conf_upgate = 1;

    _cfg->dev->ctr.trans_start = 1;
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
