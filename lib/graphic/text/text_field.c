#include "text_field.h"

// надо чтобы возвращаемое значение показывало получилось ли втиснуться в координаты
unsigned tf_ctx_calc(tf_ctx_t * ctx, form_t * f, const tf_cfg_t * cfg)
{
    ctx->tfcfg = cfg;
    unsigned scale = cfg->fcfg->scale;
    if (scale == 0) {
        scale = 1;
    }

    for (unsigned d = 0; d < DIMENSION_COUNT; d++) {
        unsigned gap = cfg->fcfg->gaps.ca[d];
        if (gap == 0) {
            gap = 1;
            // охуенный план также расширять gaps и scale если там нули
            // и у тебя упрется по одной из координат всегда текст, и другую координату можно будет уменьшить
        }
        unsigned char_linear_size = cfg->fcfg->font->size.ca[d];

        unsigned text_len = 0;
        unsigned text_linear_size = 0;

        unsigned form_size_px = f->s.ca[d];
        unsigned available_px = form_size_px - (2 * cfg->padding.ca[d]);
        unsigned available_chars = (available_px + gap) / ((char_linear_size * scale) + gap);

        if (cfg->limit_char.ca[d]) {
            // вариант когда мы уменьшаем размер формы
            text_len = cfg->limit_char.ca[d];
            if (text_len > available_chars) {
                // если нужно больше места то возвращаем 0, чтобы list понял что форма не влезла
                return 0;
            }
        } else {
            // вариант когда мы расширяем по максимуму
            text_len = available_chars;
        }

        ctx->size.ca[d] = text_len;
        text_linear_size = (char_linear_size * text_len * scale) + ((text_len - 1) * gap);

        // перезаписываем размер по данной координате в форме
        // - надо ли ? основной вопрос
        if (cfg->limit_char.ca[d]) {
            form_size_px = text_linear_size + (2 * cfg->padding.ca[d]);
            f->s.ca[d] = form_size_px;
        }

        // align
        ctx->xy.ca[d] = f->p.ca[d];
        if (cfg->a.ca[d].center) {
            ctx->xy.ca[d] += (f->s.ca[d] - text_linear_size) / 2;
        } else {
            if (cfg->a.ca[d].edge == EDGE_L) {
                ctx->xy.ca[d] += cfg->padding.ca[d];
            } else {
                ctx->xy.ca[d] += f->s.ca[d] - cfg->padding.ca[d] - text_linear_size;
            }
        }
    }

   return 1;
}
