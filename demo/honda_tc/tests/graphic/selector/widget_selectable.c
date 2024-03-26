#include "widget_selectable.h"
#include "widget_test_rect_with_text.h"

typedef struct {
    uint8_t selected;
    uint8_t n;
    ui_node_desc_t child_node;
    __widget_test_rect_with_text_cfg_t child_cfg;
    char text[3];
} __widget_selectable_ctx_t;

static void update(ui_element_t * el)
{
    __widget_selectable_ctx_t * ctx = (__widget_selectable_ctx_t *)el->ctx;
    printf("update selectable: %s\n", ctx->text);
    if (ctx->selected == 0) {
        ctx->child_cfg.cs.bg = 0xA04020;
    } else {
        ctx->child_cfg.cs.bg = 0x708090;
    }
    ctx->text[1] = '1' + ctx->n;
    printf("update selectable end: %s\n", ctx->text);

    ui_element_t * item = ui_tree_child(el);
    ui_tree_element_draw(item);
}

static void draw(ui_element_t * el)
{
    printf("draw selectable\n");
    __widget_selectable_ctx_t * ctx = (__widget_selectable_ctx_t *)el->ctx;

    ctx->child_node.cfg = &ctx->child_cfg;
    ctx->child_node.widget = &__widget_test_rect_with_text;

    ctx->n = 0;
    ctx->selected = 0;
    ctx->child_cfg.cs.fg = 0x23fa75;
    ctx->child_cfg.text = ctx->text;
    ctx->text[0] = 'A' + el->idx;
    ctx->text[2] = 0;


    ui_element_t * item = ui_tree_add(el, &ctx->child_node);
    item->f = el->f;

    update(el);
}

static unsigned process(ui_element_t * el, unsigned event)
{
    __widget_selectable_ctx_t * ctx = (__widget_selectable_ctx_t *)el->ctx;

    if (event == 10) {
        if (ctx->selected == 0) {
            ctx->selected = 1;
        } else {
            ctx->selected = 0;
        }
        update(el);
        return 1;
    }

    if (ctx->selected == 1) {
        if (event == 1) {
            ctx->n++;
            if (ctx->n == 10) {
                ctx->n = 0;
            }
            update(el);
            return 1;
        }
        if (event == 2) {
            if (ctx->n == 0) {
                ctx->n = 10;
            }
            ctx->n--;
            update(el);
            return 1;
        }
    }
    return 0;
}

widget_desc_t __widget_selectable = {
    .draw = draw,
    .process_event = process,
    .ctx_size = sizeof(__widget_selectable_ctx_t)
};
