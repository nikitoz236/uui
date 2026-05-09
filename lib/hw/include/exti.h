#pragma once
#include <stdint.h>
#include "gpio.h"
#include "ll.h"

/*
    обёртка над EXTI - внешними прерываниями GPIO.

    группа = gpio_list (пины) + параллельный массив edge-настроек
    (rise/fall) + колбэк. Регистрируется через exti_add_group.
    Количество линий берётся из pins->count.

    группа из одной линии - on_edge_single(state) (без idx).
    группа из нескольких - on_edge_idx(idx, state).

    ll_item должен быть первым полем для каста в ll_item_t.

    реализация под конкретный МК - в lib/hw/uc_hw/<plat>/periph_exti.c
    (STM32F1, STM32F0). Платформенная специфика: ограничения по линиям,
    маппинг IRQ, регистры AFIO/SYSCFG.
*/

typedef struct __attribute__((packed)) {
    uint8_t fall : 1;
    uint8_t rise : 1;
} exti_edge_t;

typedef struct {
    ll_item_t ll_item;
    const gpio_list_t * pins;
    const exti_edge_t * edges;
    union {
        void (*on_edge_idx)(unsigned idx, unsigned state);
        void (*on_edge_single)(unsigned state);
    };
} exti_group_t;

void exti_add_group(exti_group_t * group);
