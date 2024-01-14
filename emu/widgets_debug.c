#include <stdio.h>
#include "widgets.h"

void widgets_print_ctx(unsigned level, ui_ctx_t * ctx, unsigned max_num)
{
    for (unsigned i = 0; i < level; i++) {
        printf("  ");
    }
    int len = childs_ctx_size(ctx);
    ui_ctx_t * child_ctx = first_child(ctx);

    printf("node: %p, child_offset: %d, size: %d, idx: %d, child size: %d, ", ctx->node, ctx->child_offset, ctx->size, ctx->idx, len);
    printf("pos: (%d, %d), size: (%d, %d)\r\n",ctx->f.p.x, ctx->f.p.y, ctx->f.s.w, ctx->f.s.h);

    unsigned num = max_num;
    while ((len > 0) && (num > 0))  {
        widgets_print_ctx(level + 1, child_ctx, max_num);
        len -= child_ctx->size;

        for (unsigned i = 0; i < level; i++) {
           printf("  ");
        }
        printf("--len childs rem: %ld\r\n", len);

        num--;
        child_ctx = next_child(child_ctx);
    }
}
