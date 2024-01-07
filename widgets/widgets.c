#include "widgets.h"

void process_node(ui_ctx_t * ctx, unsigned event)
{
    unsigned result = 0;
    if (ctx->node->widget->process) {
        result = ctx->node->widget->process(ctx, event);
    }
    if (result) {
        event = 0;
    }
    unsigned len = childs_ctx_size(ctx);
    ui_ctx_t * child_ctx = first_child(ctx);
    while (len) {
        process_node(child_ctx, event);
        len -= child_ctx->size;
        child_ctx = next_child(child_ctx);
    }
}
