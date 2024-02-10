#pragma once
#include <stdint.h>
#include "forms.h"

typedef struct __attribute__((packed)) {
    unsigned center : 1;
    enum {
        ALIGN_U,
        ALIGN_D,
        ALIGN_L = ALIGN_U,
        ALIGN_R = ALIGN_D,
    } edge : 1;
    enum {
        ALIGN_O,
        ALIGN_I,
    } side : 1;
} alignment_t;

typedef struct __attribute__((packed)) {
    uint8_t margin[2];
    alignment_t align;
} align_one_dir_t;

typedef struct __attribute__((packed)) {
    struct {
        uint8_t ref_object_idx;
        align_one_dir_t ap;
    } fa[2];
} align_list_item_t;

#define __GROUP_ALIGNMENT_C             { .center = 1 }
#define __GROUP_ALIGNMENT_LO            { .edge = ALIGN_L, .side = ALIGN_O }
#define __GROUP_ALIGNMENT_LI            { .edge = ALIGN_L, .side = ALIGN_I }
#define __GROUP_ALIGNMENT_RO            { .edge = ALIGN_R, .side = ALIGN_O }
#define __GROUP_ALIGNMENT_RI            { .edge = ALIGN_R, .side = ALIGN_I }
#define __GROUP_ALIGNMENT_UO            __GROUP_ALIGNMENT_LO
#define __GROUP_ALIGNMENT_UI            __GROUP_ALIGNMENT_LI
#define __GROUP_ALIGNMENT_DO            __GROUP_ALIGNMENT_RO
#define __GROUP_ALIGNMENT_DI            __GROUP_ALIGNMENT_RI

#define GROUP_ALIGN_ITEM(ha, hm1, hm2, hri, va, vm1, vm2, vri) \
    { \
        .fa = { \
            { .ref_object_idx = hri, .ap = { .margin = { hm1, hm2 }, .align = __GROUP_ALIGNMENT_ ## ha } }, \
            { .ref_object_idx = vri, .ap = { .margin = { vm1, vm2 }, .align = __GROUP_ALIGNMENT_ ## va } }, \
        } \
    }

/*
    мы имеем r_list - описание выравнивания n объектов в группе

    1. создаем массив форм объектов в группе на 1 больше чем объектов. 0 будет результирующей формой
    2. вычисляем для форм объектов их размеры каким либо образом, начиная с 1
    3. вызов aligned_group_calc_size вычислит нам размер 0 формы - размер всей группы
    4. выполняем позиционирование 0 формы (например также через r_list), можем изменить ее размер
    5. вызов aligned_group_calc_pos вычислит нам все положения форм объектов в группе
*/

void aligned_group_calc_size(form_t form_list[], const align_list_item_t r_list[], unsigned n);
void aligned_group_calc_pos(form_t form_list[], const align_list_item_t r_list[], unsigned n);
