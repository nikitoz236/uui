#include "str_val_buf.h"
#include "text_label_color.h"
#include "lcd_text_color.h"
#include "str_utils.h"

// #include <stdio.h>

static unsigned update_ctx_val(void * ctx, void * new, val_rep_t rep, unsigned offset)
{
    static const uint8_t vl[] = {
        [VAL_SIZE_8] = 1,
        [VAL_SIZE_16] = 2,
        [VAL_SIZE_32] = 4,
    };
    if (!str_cmp(ctx + offset, new + offset, vl[rep.vs])) {
        str_cp(ctx + offset, new + offset, vl[rep.vs]);
        return 1;
    }
    return 0;
}

static const char * str_from_val(void * ptr, val_rep_t rep, val_text_t vt, unsigned len)
{
    const char * str = str_val_buf_get();
    val_text_ptr_to_str((char *)str, ptr, rep, vt, len);
    return str;
}

/*
    все! мне для того чтобы выдергивать форматирование значений для компьютера понадобился индекс здесь, можно конечно было через контекст, но это память

    страшно конечно выглдяит. но пофиг
    итого можно перенести label_static_print в одну функцию с label_value_print


    нам надо отвечать всегда на вопросы:
        есть ли значение ?
            как получить ?
                контекст и оффсет
                функция по индексу
            будем сравнивать ?
                контекст и оффсет
            будем ли мы сравнивать контексты

        как получить текст
            константный
                текст
                функция по индексу
                массив по индексу
            преобразовать из значения
                функция по значению
                десятичное представление

        где его расположить
            текстовое поле и его координаты

        каким цветом
*/

//  label print
//  const tf_ctx_t * tf         контекст текстового поля
//  const label_value_t * l     структура с описанием того что и как печатать
//  color_scheme_t * cs         цветовая схема
//  void * prev_ctx             контекст в котором хранится предыдущее значение, для сравнения
//  void * ctx                  контекст в котором хранится значение
//  unsigned idx                индекс однотипного компонента
void lp(const tf_ctx_t * tf, const label_t * l, color_scheme_t * cs, void * prev_ctx, void * ctx, unsigned idx)
{
    unsigned t = l->t;
    const char * str = 0;
    // идея в двух контекстах, в одном значения которые мы печатаем, в другом
    // предыдущие значения с которыми сверяем необходимость обновления.

    if (t == LP_T) {
        str = l->text;
    } else if (t == LP_T_FIDX) {
        str = l->to_str(idx);
    } else if (t == LP_T_LIDX) {
        str = l->text_list[idx];
    } else {
        if (prev_ctx) {
            // если указан контекст с которым сравнивать, сравниваем и если значение поменялось то продолжаем
            if (!update_ctx_val(prev_ctx, ctx, l->rep, l->ofs)) {
                return;
            }
        }

        if (t == LP_V) {
            str = str_from_val(ctx + l->ofs, l->rep, l->vt, l->len);
        } else {
            unsigned v = val_unpack(ctx + l->ofs, l->rep, 0);

            if (t == LP_S) {
                void * sub_ctx = ctx + l->sofs;
                void * sub_prev_ctx = 0;
                if (prev_ctx) {
                   sub_prev_ctx = prev_ctx + l->sofs;
                }
                if (l->rep.s) {
                    int sv = val_ptr_to_signed(ctx + l->ofs, l->rep.vs);
                    l->sl->ctx_update_signed(sub_ctx, sv);
                } else {
                    l->sl->ctx_update(sub_ctx, v);
                }
                for (unsigned i = 0; i < l->sl->count; i++) {
                    lp(tf, &l->sl->list[i], cs, sub_prev_ctx, sub_ctx, idx);
                }
                // lp_color(tf, cs->bg, l->sl, idx, sub_ctx, sub_prev_ctx);
                return;
            } else if (t == LP_T_LV) {
                str = l->text_list[v];
            } else if (t == LP_T_FV) {
                str = l->to_str(v);
            } else if (t == LP_V_FIDX) {
                val_text_t vt;
                l->vt_by_idx(&vt, idx);
                str = str_from_val(ctx + l->ofs, l->rep, vt, l->len);
            }
        }
    }

    lcd_color_text_raw_print(str, tf->tfcfg->fcfg, cs, &tf->xy, &tf->size, &l->xy, l->len);
}

void label_color(const tf_ctx_t * tf, const label_color_t * label, lcd_color_t bg_color, unsigned idx, void * ctx, void * prev_ctx)
{
    color_scheme_t cs = { .bg = bg_color, .fg = label->color };
    lp(tf, &label->l, &cs, prev_ctx, ctx, idx);
}

void label_color_list(const tf_ctx_t * tf, const label_list_t * list, lcd_color_t bg_color, unsigned idx, void * ctx, void * prev_ctx)
{
    color_scheme_t cs = { .bg = bg_color };
    if (list->ctx_update) {
        list->ctx_update(ctx, idx);
    }
    for (unsigned i = 0; i < list->count; i++) {
        label_color_t * a = (label_color_t *)list->wrap_list;
        cs.fg = a[i].color;
        lp(tf, &a[i].l, &cs, prev_ctx, ctx, idx);
    }
}

void lcd_label_list(const tf_ctx_t * tf, const label_list_t * list, const color_scheme_t * cs, unsigned idx, void * ctx, void * prev_ctx)
{
    if (list->ctx_update) {
        list->ctx_update(ctx, idx);
    }
    for (unsigned i = 0; i < list->count; i++) {
        lp(tf, &list->list[i], cs, prev_ctx, ctx, idx);
    }
}
