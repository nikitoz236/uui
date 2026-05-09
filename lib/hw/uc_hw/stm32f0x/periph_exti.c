#include "exti.h"
#include "periph_gpio.h"
#include "periph_header.h"
#include "irq_vectors.h"

static ll_item_t * groups_head = 0;
static exti_group_t * line_to_group[16] = {};
static uint8_t line_to_idx[16] = {};

static void handle_line(unsigned line)
{
    unsigned mask = 1 << line;
    if (!(EXTI->PR & mask)) {
        return;
    }
    EXTI->PR = mask;
    exti_group_t * g = line_to_group[line];
    if (!g) {
        return;
    }
    unsigned idx = line_to_idx[line];
    unsigned state = stm_gpio_get_state(g->pins->pin_list[idx]);
    if (g->pins->count == 1) {
        g->on_edge_single(state);
    } else {
        g->on_edge_idx(idx, state);
    }
}

static void exti_irq_0_1(void)
{
    for (unsigned line = 0; line <= 1; line++) {
        handle_line(line);
    }
}

static void exti_irq_2_3(void)
{
    for (unsigned line = 2; line <= 3; line++) {
        handle_line(line);
    }
}

static void exti_irq_4_15(void)
{
    for (unsigned line = 4; line <= 15; line++) {
        handle_line(line);
    }
}

static void configure_line(gpio_pin_t pin, unsigned rise, unsigned fall)
{
    unsigned line = pin.pin;
    unsigned port = pin.port - 1;
    unsigned reg = line / 4;
    unsigned shift = (line % 4) * 4;

    SYSCFG->EXTICR[reg] &= ~(0xF << shift);
    SYSCFG->EXTICR[reg] |= port << shift;

    if (rise) {
        EXTI->RTSR |= (1 << line);
    }
    if (fall) {
        EXTI->FTSR |= (1 << line);
    }
    EXTI->IMR |= (1 << line);

    IRQn_Type irqn;
    void (*handler)(void);
    if (line >= 4) {
        irqn = EXTI4_15_IRQn;
        handler = exti_irq_4_15;
    } else if (line >= 2) {
        irqn = EXTI2_3_IRQn;
        handler = exti_irq_2_3;
    } else {
        irqn = EXTI0_1_IRQn;
        handler = exti_irq_0_1;
    }
    NVIC_SetHandler(irqn, handler);
    NVIC_EnableIRQ(irqn);
}

void exti_add_group(exti_group_t * group)
{
    ll_add(&groups_head, (ll_item_t *)group);
    for (unsigned i = 0; i < group->pins->count; i++) {
        gpio_pin_t pin = group->pins->pin_list[i];
        unsigned line = pin.pin;
        line_to_group[line] = group;
        line_to_idx[line] = i;
        configure_line(pin, group->edges[i].rise, group->edges[i].fall);
    }
}
