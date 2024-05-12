
struct gpio_pin;
struct gpio_cfg;

typedef struct gpio_pin gpio_pin_t;
typedef struct gpio_cfg gpio_cfg_t;

void gpio_set_cfg(gpio_pin_t * pin, gpio_cfg_t * cfg);

void gpio_set_state(gpio_pin_t * pin, unsigned state);
unsigned gpio_get_state(gpio_pin_t * pin);
