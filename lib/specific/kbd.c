#include "kbd.h"
#include "gpio.h"
#include "bit_fields.h"

static const kbd_cfg_t * kbd_cfg = 0;

void init_kbd(const kbd_cfg_t * cfg)
{
    kbd_cfg = cfg;
    init_gpio_list(cfg->cols);
    if (cfg->rows) {
        init_gpio_list(cfg->rows);
    }
}

void kbd_scan_linear(void (*change)(unsigned num, unsigned state), unsigned offset)
{
    for (unsigned col = 0; col < gpio_list_count(kbd_cfg->cols); col++) {
        // кнопка нажата - линия замкнута на землю
        unsigned val = gpio_list_get_state(kbd_cfg->cols, col);
        if (kbd_cfg->col_inv) {
            val = !val;
        }

        // по началу содержит 0
        unsigned last = bf_get(kbd_cfg->prev_state, offset + col, 1);
        if (last != val) {
            change(col, val);
            bf_set(kbd_cfg->prev_state, offset + col, 1, val);
        }
    }
}

void kbd_scan(void (*change)(unsigned num, unsigned state))
{
    if (kbd_cfg->rows == 0) {
        kbd_scan_linear(change, 0);
    }
}



// static void keyboard_set_row(unsigned row)
// {
//     unsigned mask = 1;
//     for (unsigned i = 0; i < row_dec.count; i++) {
//         gpio_list_set_state(&row_dec, i, row & mask);
//         mask <<= 1;
//     }
// }

// void __kbd_scan(void (*change)(unsigned num, unsigned state))
// {
//     unsigned rows = 1;
//     if (kbd_cfg->rows) {
//         unsigned row_pins = gpio_list_count(kbd_cfg->rows);
//         if (kbd_cfg->row_dec) {
//             rows = 1 << row_pins;
//         } else {
//             rows = row_pins;
//         }
//     }
//     unsigned idx_base = 0;

//     for (unsigned r = 0; r < rows; r++) {
//         keyboard_set_row(r);
//         for (unsigned c = 0; c < col_in.count; c++) {
//             unsigned val = gpio_list_get_state(&col_in, c);
//             unsigned last = 1;
//             if (mask & prev[c]) {
//                 last = 0;
//             }
//             if (last != val) {
//                 change(idx_base + c, !val);
//                 if (val) {
//                     prev[c] &= ~mask;
//                 } else {
//                     prev[c] |= mask;
//                 }
//             }
//         }
//         idx_base += col_in.count;
//         // mask <<= 1;
//     }
// }
