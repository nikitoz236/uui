
struct gpio_pin;
struct gpio_cfg;

typedef struct gpio_pin gpio_pin_t;
typedef struct gpio_cfg gpio_cfg_t;

void gpio_set_cfg(const gpio_pin_t * pin, const gpio_cfg_t * cfg);

void gpio_set_state(const gpio_pin_t * pin, unsigned state);
unsigned gpio_get_state(const gpio_pin_t * pin);
