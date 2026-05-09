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

static void exti_irq_0(void)    { handle_line(0); }
static void exti_irq_1(void)    { handle_line(1); }
static void exti_irq_2(void)    { handle_line(2); }
static void exti_irq_3(void)    { handle_line(3); }
static void exti_irq_4(void)    { handle_line(4); }

static void exti_irq_9_5(void)
{
    for (unsigned line = 5; line <= 9; line++) {
        handle_line(line);
    }
}

static void exti_irq_15_10(void)
{
    for (unsigned line = 10; line <= 15; line++) {
        handle_line(line);
    }
}

static void configure_line(gpio_pin_t pin, unsigned rise, unsigned fall)
{
    unsigned line = pin.pin;
    unsigned port = pin.port - 1;
    unsigned reg = line / 4;
    unsigned shift = (line % 4) * 4;

    AFIO->EXTICR[reg] &= ~(0xF << shift);
    AFIO->EXTICR[reg] |= port << shift;

    if (rise) {
        EXTI->RTSR |= (1 << line);
    }
    if (fall) {
        EXTI->FTSR |= (1 << line);
    }
    EXTI->IMR |= (1 << line);

    static const IRQn_Type irqn_table[5] = {
        EXTI0_IRQn, EXTI1_IRQn, EXTI2_IRQn, EXTI3_IRQn, EXTI4_IRQn,
    };
    static void (* const handler_table[5])(void) = {
        exti_irq_0, exti_irq_1, exti_irq_2, exti_irq_3, exti_irq_4,
    };
    IRQn_Type irqn;
    void (*handler)(void);
    if (line >= 10) {
        irqn = EXTI15_10_IRQn;
        handler = exti_irq_15_10;
    } else if (line >= 5) {
        irqn = EXTI9_5_IRQn;
        handler = exti_irq_9_5;
    } else {
        irqn = irqn_table[line];
        handler = handler_table[line];
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
