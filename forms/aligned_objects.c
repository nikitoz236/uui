#include "aligned_objects.h"

//  ref_object_idx          индекс формы в form_list, которая имеет описание выравнивания r_list[ref_object_idx]
//  n                       количество именно объектов группы, то есть размер массива r_list, на 1 меньше чем размер form_list
static unsigned object_with_referenced_size(unsigned ref_object_idx, unsigned c, form_t form_list[], const align_list_item_t r_list[], unsigned n)
{
    static const align_one_dir_t default_align = { .margin = { 0, 0 } };

    align_one_dir_t * ref_object_dir_align = &default_align;
    unsigned ref_size = 0;
    if (ref_object_idx != 0) {
        ref_object_dir_align = &r_list[ref_object_idx - 1].fa[c].ap;
        ref_size = form_list[ref_object_idx].s.ca[c];
    }

    unsigned extend_size[2] = { ref_object_dir_align->margin[0], ref_object_dir_align->margin[1] };
    unsigned max_center_object_total_size = 0;
    for (int i = 0; i < n; i++) {
        if (r_list[i].fa[c].ref_object_idx == ref_object_idx) {
            align_one_dir_t * object_dir_align = &r_list[i].fa[c].ap;
            unsigned object_total_size = object_with_referenced_size(i + 1, c, form_list, r_list, n);

            if (object_dir_align->align.center) {
                if (max_center_object_total_size < object_total_size) {
                    max_center_object_total_size = object_total_size;
                }
            } else {
                unsigned ref_edge = object_dir_align->align.edge;
                unsigned object_edge = !ref_edge;
                if (ref_object_idx == 0) {
                    if (object_dir_align->align.side == ALIGN_I) {
                        // если все объекты внутри вычисляемого размера
                        // то размер объекта с внешним отступом идет в общий размер extend_size
                        if (extend_size[ref_edge] < (object_total_size - object_dir_align->margin[object_edge])) {
                            extend_size[ref_edge] = (object_total_size - object_dir_align->margin[object_edge]);
                        }
                        // а внуренние отступы идут в размер серединки вычисляемого объекта
                        if (ref_size < object_dir_align->margin[object_edge]) {
                            ref_size = object_dir_align->margin[object_edge];
                        }
                    } else {
                        // не должно быть объектов снаружи формы. не учитываем их размер
                    }
                } else {
                    if (object_dir_align->align.side == ALIGN_I) {
                        if (object_total_size > (ref_size + extend_size[object_edge])) {
                            extend_size[object_edge] = object_total_size - ref_size;
                        }
                    } else {
                        unsigned common_margin = ref_object_dir_align->margin[ref_edge];
                        if (common_margin < object_dir_align->margin[object_edge]) {
                            common_margin = object_dir_align->margin[object_edge];
                        }
                        unsigned extend = common_margin + object_total_size - object_dir_align->margin[object_edge];
                        if (extend_size[ref_edge] < extend) {
                            extend_size[ref_edge] = extend;
                        }
                    }
                }
            }
        }
    }

    unsigned ref_total_size = ref_size + extend_size[0] + extend_size[1];

    if (ref_total_size < max_center_object_total_size) {
        ref_total_size = max_center_object_total_size;
    }

    return ref_total_size;
}

void aligned_group_calc_size(form_t form_list[], const align_list_item_t r_list[], unsigned n)
{
    for (unsigned c = 0; c < 2; c++) {
        form_list[0].s.ca[c] = object_with_referenced_size(0, c, form_list, r_list, n);
    }
}

void aligned_group_calc_pos(form_t form_list[], const align_list_item_t r_list[], unsigned n)
{
    for (unsigned c = 0; c < 2; c++) {
        unsigned box_size = form_list[0].s.ca[c];
        unsigned box_pos = form_list[0].p.ca[c];
        for (unsigned ref_idx = 0; ref_idx < n; ref_idx++) {
            for (unsigned i = 0; i < n; i++) {
                if (r_list[i].fa[c].ref_object_idx == ref_idx) {
                    align_one_dir_t * object_dir_align = &r_list[i].fa[c].ap;
                    unsigned object_size = form_list[i + 1].s.ca[c];
                    unsigned object_offset = 0;

                    if (object_dir_align->align.center) {
                        object_offset = (box_size - object_size) / 2;
                    } else {
                        if (object_dir_align->align.side == ALIGN_I) {
                            if (object_dir_align->align.edge == ALIGN_L) {
                                object_offset = object_dir_align->margin[0];
                            } else {
                                object_offset = box_size - object_dir_align->margin[1] - object_size;
                            }
                        } else {
                            // такого не должно быть !!! все объекты будут свалены в кучу слева сверху
                        }
                    }

                    unsigned result_pos = box_pos + object_offset;
                    form_list[i + 1].p.ca[c] = result_pos;
                }
            }
        }
    }
}
