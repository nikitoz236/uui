#pragma once

struct gpio;
typedef struct gpio gpio_t;

void init_gpio(const gpio_t * gpio);
void gpio_set_state(const gpio_t * gpio, unsigned state);
unsigned gpio_get_state(const gpio_t * gpio);

struct gpio_list;
typedef struct gpio_list gpio_list_t;

void init_gpio_list(const gpio_list_t * gpio_list);
void gpio_list_set_state(const gpio_list_t * gpio_list, unsigned idx, unsigned state);
unsigned gpio_list_get_state(const gpio_list_t * gpio_list, unsigned idx);
