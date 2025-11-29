#include "esp32_gpio.h"

#include "soc/io_mux_reg.h"
#include "soc/gpio_struct.h"

typedef union {
    struct {
        uint32_t mcu_oe : 1;
        uint32_t slp_sel : 1;
        uint32_t mcu_wpd : 1;
        uint32_t mcu_wpu : 1;

        uint32_t mcu_ie : 1;
        uint32_t mcu_drv : 2;
        uint32_t fun_wpd : 1;

        uint32_t fun_wpu : 1;
        uint32_t fun_ie : 1;
        uint32_t fun_drv : 2;

        uint32_t mcu_sel : 3;
        uint32_t filter_en : 1;
    };
    uint32_t val;
} iomux_gpio_t;

typedef struct {
    volatile uint32_t pin_ctrl;
    volatile iomux_gpio_t gpio[49];
} iomux_dev_t;

#define IOMUX           ((iomux_dev_t *)(DR_REG_IO_MUX_BASE))

void iomux_set_func(unsigned pin, unsigned func, unsigned pu, unsigned pd)
{
    iomux_gpio_t tmp;
    tmp.val = IOMUX->gpio[pin].val;
    tmp.fun_wpd = pd;
    tmp.fun_wpu = pu;
    tmp.fun_ie = 1;
    tmp.mcu_sel = func;
    IOMUX->gpio[pin].val = tmp.val;
}

typedef struct {
    uint32_t val;
    uint32_t set;
    uint32_t clr;
} gpio_rs_reg;

gpio_rs_reg * gpio_dir = (gpio_rs_reg *)&GPIO.enable;
gpio_rs_reg * gpio_odr = (gpio_rs_reg *)&GPIO.out;

static void init_gpio_pin(const gpio_pin_t * pin, const gpio_cfg_t * cfg)
{
    gpio_mode_t mode = cfg->mode;
    unsigned idx = pin->pin >> 5;
    unsigned bit = pin->pin & 0x1F;

    unsigned func = 1;
    if (mode == GPIO_MODE_MUX) {
        func = pin->signal;
    }
    iomux_set_func(pin->pin, func, cfg->pu, cfg->pd);

    if (mode == GPIO_MODE_OUT) {
        // надо MUX перенастроить такто на FUNCTION 1
        gpio_dir[idx].set = 1 << bit;
    }

    if (mode == GPIO_MODE_IN) {
        gpio_dir[idx].clr = 1 << bit;
    }

    if ((mode == GPIO_MODE_SIG_IN) || (mode == GPIO_MODE_SIG_IO)) {
        GPIO.func_in_sel_cfg[pin->signal].func_sel = pin->pin;
    }

    if ((mode == GPIO_MODE_SIG_OUT) || (mode == GPIO_MODE_SIG_IO)) {
        GPIO.func_out_sel_cfg[pin->pin].func_sel = pin->signal;
    }
}

static void gpio_pin_set_state(const gpio_pin_t * pin, unsigned state)
{
    unsigned idx = pin->pin >> 5;
    unsigned bit = pin->pin & 0x1F;
    if (state) {
        gpio_odr[idx].set = 1 << bit;
    } else {
        gpio_odr[idx].clr = 1 << bit;
    }
}

static unsigned gpio_pin_get_state(const gpio_pin_t * pin)
{
    uint32_t idr;
    if (pin->pin < 32) {
        idr = GPIO.in;
    } else {
        idr = GPIO.in1.val;
    }
    unsigned bit = pin->pin & 0x1F;
    if (idr & (1 << bit)) {
        return 1;
    }
    return 0;
}

void init_gpio(const gpio_t * gpio)
{
    init_gpio_pin(&gpio->pin, &gpio->cfg);
}

void gpio_set_state(const gpio_t * gpio, unsigned state)
{
    gpio_pin_set_state(&gpio->pin, state);
}

unsigned gpio_get_state(const gpio_t * gpio)
{
    return gpio_pin_get_state(&gpio->pin);
}

void init_gpio_list(const gpio_list_t * gpio_list)
{
    for (unsigned i = 0; i < gpio_list->count; i++) {
        init_gpio_pin(&gpio_list->pin_list[i], &gpio_list->cfg);
    }
}

void gpio_list_set_state(const gpio_list_t * gpio_list, unsigned idx, unsigned state)
{
    gpio_pin_set_state(&gpio_list->pin_list[idx], state);
}

unsigned gpio_list_get_state(const gpio_list_t * gpio_list, unsigned idx)
{
    return gpio_pin_get_state(&gpio_list->pin_list[idx]);
}

unsigned gpio_list_count(const gpio_list_t * gpio_list)
{
    return gpio_list->count;
}
