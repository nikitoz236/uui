#include "aligned_objects.h"

// #include "stdarg.h"
// static void dbg_print(unsigned level, const char * format, ... )
// {
//     while (level--) {
//         printf("    ");
//     }
//     va_list args;
//     va_start(args, format);
//     printf(format, args);
//     va_end(args);
// }

#define dbg_print(level, format, ...) \
    for (int i = 0; i < level; i++) { printf("    "); }; \
    printf(format, ##__VA_ARGS__)

//  ref_object_idx          индекс формы в form_list, которая имеет описание выравнивания r_list[ref_object_idx]
//  n                       количество именно объектов группы, то есть размер массива r_list, на 1 меньше чем размер form_list
static unsigned object_with_referenced_size(unsigned ref_object_idx, unsigned c, form_t form_list[], const align_list_item_t r_list[], unsigned n)
{
    static const align_one_dir_t default_align = { .margin = { 0, 0 } };

    const align_one_dir_t * ref_object_dir_align = &default_align;
    unsigned ref_size = 0;
    if (ref_object_idx != 0) {
        ref_object_dir_align = &r_list[ref_object_idx - 1].fa[c].ap;
        ref_size = form_list[ref_object_idx].s.ca[c];
    }

    dbg_print(ref_object_idx + 1, "coord: %d, ref_object_idx %d, ref_size %d\r\n", c, ref_object_idx, ref_size);

    unsigned extend_size[2] = { ref_object_dir_align->margin[0], ref_object_dir_align->margin[1] };
    unsigned max_center_object_total_size = 0;
    for (int i = 0; i < n; i++) {
        if (r_list[i].fa[c].ref_object_idx == ref_object_idx) {
            const align_one_dir_t * object_dir_align = &r_list[i].fa[c].ap;

            dbg_print(ref_object_idx + 1, "-start calc size for object %d align c %d \n", i, object_dir_align->align.center);
            unsigned object_total_size = object_with_referenced_size(i + 1, c, form_list, r_list, n);
            dbg_print(ref_object_idx + 1, "--object size %d\n", object_total_size);

            if (object_dir_align->align.center) {
                dbg_print(ref_object_idx + 1, "--align center size\n");

                if (max_center_object_total_size < object_total_size) {
                    max_center_object_total_size = object_total_size;
                    dbg_print(ref_object_idx + 1, "--new max center size %d\n", max_center_object_total_size);
                }
            } else {
                unsigned ref_edge = object_dir_align->align.edge;
                unsigned object_edge = !ref_edge;
                if (ref_object_idx == 0) {
                    if (object_dir_align->align.side == ALIGN_I) {
                        // если все объекты внутри вычисляемого размера
                        // то размер объекта с внешним отступом идет в общий размер extend_size
                        dbg_print(ref_object_idx + 1, "--first element in group. alignment: inside ref %d, edge: %d ", ref_object_idx, ref_edge);
                        if (extend_size[ref_edge] < (object_total_size - object_dir_align->margin[object_edge])) {
                            extend_size[ref_edge] = (object_total_size - object_dir_align->margin[object_edge]);
                            dbg_print(0, "new extend size: %d,  ", extend_size[ref_edge]);
                        }
                        // а внуренние отступы идут в размер серединки вычисляемого объекта
                        if (ref_size < object_dir_align->margin[object_edge]) {
                            ref_size = object_dir_align->margin[object_edge];
                            dbg_print(0, "new margin: %d -> ref_size  ", ref_size);
                        }
                        dbg_print(0, "\n");

                    } else {
                        dbg_print(ref_object_idx + 1, "--first element in group. alignment: outside\n");
                        // не должно быть объектов снаружи формы. не учитываем их размер
                    }
                } else {
                    if (object_dir_align->align.side == ALIGN_I) {
                        dbg_print(ref_object_idx + 1, "--add element to group. alignment: inside ref: %d, size %d, edge %d\n", ref_object_idx, object_total_size, object_edge);
                        if (object_total_size > (ref_size + extend_size[object_edge])) {
                            extend_size[object_edge] = object_total_size - ref_size;

                        }
                    } else {
                        unsigned common_margin = ref_object_dir_align->margin[ref_edge];
                        dbg_print(ref_object_idx + 1, "--add element to group. alignment: outside ref: %d, size %d, edge %d, common margin %d\n", ref_object_idx, object_total_size, object_edge, common_margin);
                        if (common_margin < object_dir_align->margin[object_edge]) {
                            common_margin = object_dir_align->margin[object_edge];
                            dbg_print(ref_object_idx + 1, "----margin updated\n");
                        }

                        unsigned extend = common_margin + object_total_size - object_dir_align->margin[object_edge];
                        dbg_print(ref_object_idx + 1, "----ext size for element %d\n", extend);
                        if (extend_size[ref_edge] < extend) {
                            extend_size[ref_edge] = extend;
                            dbg_print(ref_object_idx + 1, "----extend size %d updated\n", ref_edge);
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

    dbg_print(ref_object_idx + 1, "total extend size [%d, %d] ref_size %d, total_size %d\n", extend_size[0], extend_size[1], ref_size, ref_total_size);

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
        for (unsigned ref_idx = 0; ref_idx < n; ref_idx++) {
            for (unsigned i = 0; i < n; i++) {
                if (r_list[i].fa[c].ref_object_idx == ref_idx) {
                    unsigned box_size = form_list[ref_idx].s.ca[c];
                    unsigned box_pos = form_list[ref_idx].p.ca[c];

                    align_one_dir_t * object_dir_align = &r_list[i].fa[c].ap;
                    unsigned object_size = form_list[i + 1].s.ca[c];
                    int object_offset = 0;

                    dbg_print(0, "calc pos coord %d for object %d, ref %d, align center %d, edge %d, side %d, margin [%d, %d], size %d, box_size %d, box_pos %d\n",
                        c, i, ref_idx, object_dir_align->align.center, object_dir_align->align.edge, object_dir_align->align.side,
                        object_dir_align->margin[0], object_dir_align->margin[1], object_size, box_size, box_pos);

                    if (object_dir_align->align.center) {
                        object_offset = (box_size - object_size) / 2;
                        dbg_print(2, "center, object_offset %d\n", object_offset);
                    } else {
                        if (object_dir_align->align.side == ALIGN_I) {
                            if (object_dir_align->align.edge == ALIGN_L) {
                                object_offset = object_dir_align->margin[0];
                                dbg_print(2, "internal left, object_offset %d\n", object_offset);
                            } else {
                                object_offset = box_size - object_dir_align->margin[1] - object_size;
                                dbg_print(2, "internal right, object_offset %d\n", object_offset);
                            }
                        } else {
                            unsigned ref_edge = object_dir_align->align.edge;
                            unsigned object_edge = !ref_edge;

                            unsigned margin = 0;
                            if (ref_idx != 0) {
                                margin = r_list[ref_idx - 1].fa[c].ap.margin[ref_edge];
                            }
                            if (margin < object_dir_align->margin[object_edge]) {
                                margin = object_dir_align->margin[object_edge];
                            }
                            dbg_print(2, "ref edge %d, object_edge %d, margin %d\n", ref_edge, object_edge, margin);

                            if (object_dir_align->align.edge == ALIGN_L) {
                                object_offset = - margin - object_size;
                                dbg_print(2, "external left, object_offset %d\n", object_offset);
                            } else {
                                object_offset = box_size + margin;
                                dbg_print(2, "external right, object_offset %d\n", object_offset);
                            }
                        }
                    }

                    unsigned result_pos = box_pos + object_offset;
                    dbg_print(1, "box_pos %d, object_offset %d, result_pos %d\n", box_pos, object_offset, result_pos);
                    form_list[i + 1].p.ca[c] = result_pos;
                }
            }
        }
    }
}
