
#include "dp.h"

enum tlora_kbd {
    TLORA_KBD_KEY_Q,
    TLORA_KBD_KEY_W,
    TLORA_KBD_KEY_E,
    TLORA_KBD_KEY_R,
    TLORA_KBD_KEY_T,
    TLORA_KBD_KEY_Y,
    TLORA_KBD_KEY_U,
    TLORA_KBD_KEY_I,
    TLORA_KBD_KEY_O,
    TLORA_KBD_KEY_P,

    TLORA_KBD_KEY_A,
    TLORA_KBD_KEY_S,
    TLORA_KBD_KEY_D,
    TLORA_KBD_KEY_F,
    TLORA_KBD_KEY_G,
    TLORA_KBD_KEY_H,
    TLORA_KBD_KEY_J,
    TLORA_KBD_KEY_K,
    TLORA_KBD_KEY_L,
    TLORA_KBD_KEY_ENTER,

    TLORA_KBD_KEY_FN,
    TLORA_KBD_KEY_Z,
    TLORA_KBD_KEY_X,
    TLORA_KBD_KEY_C,
    TLORA_KBD_KEY_V,
    TLORA_KBD_KEY_B,
    TLORA_KBD_KEY_N,
    TLORA_KBD_KEY_M,
    TLORA_KBD_KEY_SHIFT,

    TLORA_KBD_KEY_BACKSPACE,
    TLORA_KBD_KEY_SPACE,
};

const char sym_d[] = {
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '\n',
    0,   'z', 'x', 'c', 'v', 'b', 'n', 'm', 0,
    '\b'
};

const char sym_u[] = {
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 0,
    0,   'Z', 'X', 'C', 'V', 'B', 'N', 'M', 0,
    '\b'
};

const char sym_spec[] = {
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '*', '/', '+', '-', '=', ':', '\'', '"', '@', 0,
    0,   '_', '$', ';', '?', '!', ',', '.', 0,
    '\b'
};




#include "timers_32.h"
#include "api_lcd_color.h"
#include "lcd_text_color.h"
#include "bq25896.h"
#include "esp32_systime.h"

#define CURSOR_PERIOD_MS    500

timer_32_t cursor_tim;
unsigned cursor_state = 1;

unsigned kbd_shifted = 0;
unsigned space_state = 0;
unsigned space_mod = 0;


extern font_t font_5x7;


const lcd_font_cfg_t fcfg = {
    .font = &font_5x7,
    .gaps = { .x = 4, .y = 4 },
    .scale = 1
};

color_scheme_t cs = {.bg = 0, .fg = 0xA234};
color_scheme_t cs_status = { .fg = 0xEEEE };
tptr_t tp_status;
tptr_t tp_text;

void draw_cursor(unsigned state)
{
    color_scheme_t * csp = &(color_scheme_t){.bg = COLOR(0x0ABB0C)};
    if (state == 0) {
        csp = &cs;
    }
    lcd_color_tptr_print(&tp_text, " ", *csp, 1);
    text_ptr_prev_char(&tp_text);
}


void console_draw_char(char c)
{
    dp("  char : "); dpx(c, 1); dn();

    draw_cursor(0);
    lcd_color_tptr_print(&tp_text, &c, cs, 1);
    dpn("done");
    cursor_state = 1;
    t32_run(&cursor_tim, systimer_ms(0), CURSOR_PERIOD_MS);

    draw_cursor(cursor_state);
}

void kbd_change_handler(unsigned num, unsigned state)
{
    dp("key : "); dpd(num, 2); dp(" = "); dpd(state, 1); dn();

    num--;
    if (num == TLORA_KBD_KEY_FN) {
        bq25896_power_off();
    }

    if (num ==  TLORA_KBD_KEY_SHIFT) {
        kbd_shifted = state || space_state;
        if (space_state) {
            space_mod = 1;
        }
        return;
    }

    if (num == TLORA_KBD_KEY_SPACE) {
        space_state = state;
        if (state == 0) {
            if (space_mod == 0) {
                console_draw_char(' ');
            }
            space_mod = 0;
        }
        return;
    }

    if (state) {
        const char * table = sym_d;
        if (space_state) {
            table = sym_spec;
            space_mod = 1;
        } else {
            if (kbd_shifted) {
                table = sym_u;
            }
        }
        uint8_t c = table[num];
        if (c) {
            console_draw_char(c);
        }
    }
}

// void dp_xy(const char * text, xy_t xy) { dp(text); dp(" ( "); dpd(xy.x, 5); dp(" , "); dpd(xy.y, 5); dp(" )"); }
// void dp_tp(tptr_t * tp)
// {
//     dp("tp: "); dp_xy("cpos:", tp->cpos); dp_xy(" shift ", tp->cstep); dp_xy(" pos ", tp->fxy); dp_xy(" cxy ", tp->cxy); dp_xy(" lim ", tp->tlim); dn(); 
// }

// void dp_fcfg(lcd_font_cfg_t * fcfg) {
//     dp("fcfg : "); dpx((unsigned)fcfg, 4); dp(" font "); dpx((unsigned)fcfg->font, 4); dp_xy(" gaps:", fcfg->gaps); dp(" scale "); dpd(fcfg->scale, 3); dn();
// }

void init_console(form_t * f)
{
    
    // lcd_rect(100, 100, 100, 40, 0x8F10);
    tp_text = text_ptr_create((tf_t){ .fcfg = &fcfg, .pos = f->p, .lim = fcfg_text_char_places(&fcfg, f->s) } );

    // text_ptr_init(&tp_text, &fcfg, f->p, fcfg_text_char_places(&fcfg, lcdf.s));

    // text_ptr_init(&tp, &fcfg, lcdf.p, fcfg_text_char_places(&fcfg, lcdf.s));


    // ttext_ptr_create(); init(&tp_status, &fcfg, f->p, fcfg_text_char_places(&fcfg, f->s));
    // text_ptr_set_char_pos(&tp_status, (xy_t){ .y = 1});
    // tp_text = text_ptr_export(&tp_status, (xy_t){});
    // text_ptr_set_char_pos(&tp_status, (xy_t){});
    // tp_status.tlim.y = 1;



    // lcd_color_tptr_print(&tp_status, "hey", cs, 0);
    // dpn("done");

    draw_cursor(cursor_state);
    t32_run(&cursor_tim, systimer_ms(0), CURSOR_PERIOD_MS);

}


void console_process(void)
{
    if (t32_is_over(&cursor_tim, systimer_ms(0))) {
        t32_extend(&cursor_tim, CURSOR_PERIOD_MS);
        cursor_state = !cursor_state;
        draw_cursor(cursor_state);
    }
}