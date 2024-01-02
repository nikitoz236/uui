#include <stdio.h>
#include "emu_common.h"

uint8_t ui_stack[2048];

typedef struct {
    form_t f;
    uint16_t size;
    uint16_t idx;
    uint8_t ctx[];
} ui_ctx_t;

typedef struct {
    unsigned (*calc)(void * cfg, ui_ctx_t * node_ctx);
    void (*draw)(void * cfg, ui_ctx_t * node_ctx);
} widget_t;

typedef struct {
    widget_t * widget;
    void * widget_cfg;
} ui_node_t;

// считает размер прямоугольника виджета и размер контекста, заполняет индекс
void calc_node(ui_node_t * node, ui_ctx_t * node_ctx, unsigned idx) {
    node_ctx->size = node->widget->calc(node->widget_cfg, node_ctx);
    node_ctx->idx = idx;
}

void draw_node(ui_node_t * node, ui_ctx_t * node_ctx) {
    node->widget->draw(node->widget_cfg, node_ctx);
}








typedef struct {
    xy_t size;
    unsigned color;
} rect_widget_cfg_t;

unsigned __rect_widget_calc(void * cfg, ui_ctx_t * node_ctx)
{
    node_ctx->f.s = ((rect_widget_cfg_t *)cfg)->size;
    return sizeof(ui_ctx_t);
};

void __rect_widget_draw(void * cfg, ui_ctx_t * node_ctx)
{
    emu_draw_rect(&node_ctx->f, ((rect_widget_cfg_t *)cfg)->color);
};

widget_t rect_widget = {
    .calc = __rect_widget_calc,
    .draw = __rect_widget_draw
};






typedef struct {
    ui_node_t nodes[2];
    align_mode_t align_mode;
} union_widget_cfg_t;

unsigned __union_widget_calc(void * cfg, ui_ctx_t * node_ctx) {
    union_widget_cfg_t * union_cfg = (union_widget_cfg_t *)cfg;
    ui_ctx_t * child_ctx[2];
    void * ctx_ptr = node_ctx->ctx;
    unsigned result_size = 0;

    for (int i = 0; i < 2; i++) {
        child_ctx[i] = ctx_ptr;
        calc_node(&union_cfg->nodes[i], child_ctx[i], i);
        unsigned child_node_size = child_ctx[i]->size;
        result_size += child_node_size;
        ctx_ptr += child_node_size;
    }

    form_union_calc_size(&child_ctx[0]->f, &child_ctx[1]->f, &union_cfg->align_mode, &node_ctx->f);
    return sizeof(ui_ctx_t) + result_size;
};

void __union_widget_draw(void * cfg, ui_ctx_t * node_ctx) {
    union_widget_cfg_t * union_cfg = (union_widget_cfg_t *)cfg;
    ui_ctx_t * child_ctx[2];
    void * ctx_ptr = node_ctx->ctx;
    for (int i = 0; i < 2; i++) {
        child_ctx[i] = ctx_ptr;
        unsigned child_node_size = child_ctx[i]->size;
        ctx_ptr += child_node_size;
    }
    form_union_calc_pos(&node_ctx->f, &child_ctx[0]->f, &child_ctx[1]->f, &union_cfg->align_mode);
    for (int i = 0; i < 2; i++) {
        unsigned child_idx = child_ctx[i]->idx;
        draw_node(&union_cfg->nodes[child_idx], child_ctx[i]);
    }
};

widget_t union_widget = {
    .calc = __union_widget_calc,
    .draw = __union_widget_draw
};



#define WIDGET_UNION(hm, ho, vm, vo, n1, n2)  \
    { \
        .widget = &union_widget, \
        .widget_cfg = &(union_widget_cfg_t){ \
            .nodes = { n1, n2 }, \
            .align_mode = { \
                .h = { \
                    .mode = ALIGN_ ## hm, \
                    .offset = ho \
                }, \
                .v = { \
                    .mode = ALIGN_ ## vm, \
                    .offset = vo \
                } \
            } \
        } \
    }

#define WIDGET_RECT(sw, sh, c) \
    { \
        .widget = &rect_widget, \
        .widget_cfg = &(rect_widget_cfg_t){ \
            .size = { \
                .w = sw, \
                .h = sh \
            }, \
            .color = c \
        } \
    }

#define ALIGN_MODE() \


ui_node_t ui_test =
    WIDGET_UNION( CENTER, 0, UP_OUTSIDE, 0,
        WIDGET_RECT( 200, 300, 0xff0000 ),
        WIDGET_UNION( CENTER, 0, UP_OUTSIDE, 0,
            WIDGET_RECT( 300, 50, 0x00ff00 ),
            WIDGET_UNION( CENTER, 0, UP_OUTSIDE, 0,
                WIDGET_RECT( 100, 100, 0x0000ff ),
                WIDGET_RECT( 100, 100, 0x0ffff0 )
            )
        )
    )
;




ui_node_t ui_root = {
    .widget = &union_widget,
    .widget_cfg = &(union_widget_cfg_t){
        .nodes = {
            {
                .widget = &rect_widget,
                .widget_cfg = &(rect_widget_cfg_t){
                    .size = { 200, 300 },
                    .color = 0xff0000
                }
            },
            {
                .widget = &union_widget,
                .widget_cfg = &(union_widget_cfg_t){
                    .nodes = {
                        {
                            .widget = &rect_widget,
                            .widget_cfg = &(rect_widget_cfg_t){
                                .size = { 300, 50 },
                                .color = 0x00ff00
                            }
                        },
                        {
                            .widget = &rect_widget,
                            .widget_cfg = &(rect_widget_cfg_t){
                                .size = { 100, 100 },
                                .color = 0x0000ff
                            }
                        }
                    },
                    .align_mode = {
                        .h = {
                            .mode = ALIGN_CENTER,
                            .offset = 0
                        },
                        .v = {
                            .mode = ALIGN_CENTER,
                            .offset = 0
                        }
                    }
                }
            }
        },
        .align_mode = {
            .h = {
                .mode = ALIGN_CENTER,
                .offset = 33
            },
            .v = {
                .mode = ALIGN_UP_OUTSIDE,
                .offset = 44
            }
        }
    }
};





int main() {
    ui_ctx_t * ui_ctx = (ui_ctx_t *)ui_stack;
    ui_node_t * node = &ui_test;
    calc_node(node, ui_ctx, 0);
    gfx_open(ui_ctx->f.s.x, ui_ctx->f.s.y, "UC lib gfx emu device");
    draw_node(node, ui_ctx);
    printf("ui ctx struct size: %ld\r\n", sizeof(ui_ctx_t));
    printf("ui ctx size: %d\r\n", ui_ctx->size);

    while (1) {
        char kbd_btn = gfx_routine();

        if (kbd_btn != 0) {
            printf("recieved keyboard button: %c [%d]\r\n", kbd_btn, kbd_btn);

            if (kbd_btn == 'q') {
                break;
            }
        }
        usleep(1000);
    }

    return 0;
}