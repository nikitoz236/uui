

#include "dbg_usb_cdc_acm.h"

#define DP_NOTABLE
#include "dp.h"

#include "soc/i2c_struct.h"
#include "delay_blocking.h"
#include "esp32_gpio.h"

void __debug_usart_tx_data(const char * s, unsigned len)
{
    dbg_usb_cdc_acm_tx(s, len);
}

gpio_list_t i2c_pins = {
    .count = 2,
    .cfg = {
        .mode = GPIO_MODE_SIG_IO,
        .pu = 1,
        .od = 1
    },
    .pin_list = {
        { .pin = 2, .signal = I2CEXT0_SCL_IN_IDX },
        { .pin = 3, .signal = I2CEXT0_SDA_IN_IDX }
    }
};

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

#include "esp32_pclk.h"

pclk_t i2c_pclk = SYSTEM_I2C_EXT0_CLK_EN_S;


void dump_peripheral(const char * name, void * ptr, unsigned size)
{
    dp(name); dp(" : 0x"); dpx((unsigned)ptr, 4); dp(" size: 0x"); dpx(size, 2); dn();
    unsigned offset = 0;
    while (size) {
        if ((offset & 0x0F) == 0) {}
        uint32_t val = *(uint32_t *)ptr;
        dp("    "); dpx(offset, 2); dp(" : ");
        if (val) {
            dpct(DPC_RED);
        }
        dp("0x"); dpx(val, 4);
        ptr += sizeof(uint32_t);
        size -= sizeof(uint32_t);
        offset += sizeof(uint32_t);
        dpcr();
        if ((offset & 0x0F) == 0) {
            dn();
        }
    }
    dn();
}

void calc_bus_clk(void)
{
    uint32_t source_clk = 40000000;
    uint32_t bus_freq = 400000;

    i2c_dev_t * hw = &I2C0;

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

    dp("clkm_div : "); dpd(clkm_div, 5); dn();

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




int main(void)
{
    dpn("tlora, ok");

    init_gpio_list(&i2c_pins);
    pclk_ctrl(i2c_pclk, 1);

    // dump_peripheral("I2C0", &I2C0, sizeof(I2C0));

    // I2C0.clk_conf.val = (i2c_clk_conf_reg_t){ .sclk_div_num = 25 - 1, .sclk_active = 1 }.val;

    calc_bus_clk();
    I2C0.ctr.conf_upgate = 1;


    I2C0.fifo_conf.rx_fifo_rst = 1;
    I2C0.fifo_conf.tx_fifo_rst = 1;


    I2C0.fifo_conf.rx_fifo_rst = 0;
    I2C0.fifo_conf.tx_fifo_rst = 0;

    I2C0.ctr.ms_mode = 1;
    // I2C0.ctr.clk_en = 1;
    I2C0.fifo_conf.nonfifo_en = 1;

    I2C0.ctr.conf_upgate = 1;


    // I2C0.data.val = 0x45;
    // I2C0.data.val = 0x67;


    I2C0.comd[0].val = (i2c_cmd_t){ .op_code = I2C_OP_RSTART }.val;
    I2C0.comd[1].val = (i2c_cmd_t){ .op_code = I2C_OP_WRITE, .byte_n = 1, .ack_value = 0, .ack_exp = 0, .ack_check_en = 0 }.val;
    I2C0.comd[2].val = (i2c_cmd_t){ .op_code = I2C_OP_STOP }.val;
    // I2C0.comd[3].val = (i2c_cmd_t){ .op_code = I2C_OP_END }.val;

    // I2C0.slave_addr.val = 0xAE;

    I2C0.txfifo_mem[0] = 0xEA;
    I2C0.txfifo_mem[1] = 0xBD;
    I2C0.txfifo_mem[2] = 0xEF;


    /*

            0x08
            0 101 0 000 000000 00 11 000000 0000 0000

            I2C_FIFO_ST_REG 0x14
            0b 00101 00000 00000 00101

            Register 27.22. I2C_INT_RAW_REG (0x0020)
            0b1001001000000010
        98765432109876543210

    */

    // I2C0.fifo_conf.nonfifo_en = 0;
    I2C0.ctr.conf_upgate = 1;

    // dump_peripheral("I2C0", &I2C0, sizeof(I2C0));
        delay_ms(2000);

    I2C0.ctr.trans_start = 1;
    dpn("done");

        // dump_peripheral("I2C0", &I2C0, sizeof(I2C0));
        delay_ms(2000);

        dump_peripheral("I2C0", &I2C0, sizeof(I2C0));
    while (1) {

        // dpn("fuck");
        // delay_ms(1000);
    }
}
