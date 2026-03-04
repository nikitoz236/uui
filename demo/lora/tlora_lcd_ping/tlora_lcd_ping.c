/*
формат данных на экране, scale 2

[t] send. ping seq:         1
            timeout

--- remote ---
   seq:      1     cnt:      0
  rssi:    -52     snr:     11

--- received ---
  rssi:    -48     snr:      8
*/

#include "tlora_hw.h"

#include "forms.h"
#include "font_config.h"
#include "text_pointer.h"
#include "lcd_text_color.h"
#include "api_lcd_color.h"
#include "str_val.h"

#include "lora.h"
#include "lora_ping_pong.h"
#include "delay_blocking.h"

#define PONG_TIMEOUT_MS 3000

/* ── LoRa power (XL9555 GPIO3) ───────────────────────────────────────── */

static const xl9555_gpio_t lora_pwr = {
    .addr_offset = 0,
    .pin = 3,
    .dir = XL9555_DIR_OUT,
    .polarity = XL9555_POL_NORMAL,
};

/* ── LoRa config ─────────────────────────────────────────────────────── */

static lora_cfg_t lora_cfg = {
    .chip = {
        .spi_dev = {
            .spi = &spi,
            .cs_pin = &(gpio_t){
                .pin = { .pin = 36 },
                .cfg = { .mode = GPIO_MODE_OUT },
            },
        },
        .pin = {
            [SX1262_PIN_RESET] = &(gpio_t){
                .pin = { .pin = 47 },
                .cfg = { .mode = GPIO_MODE_OUT },
            },
            [SX1262_PIN_BUSY] = &(gpio_t){
                .pin = { .pin = 48 },
                .cfg = { .mode = GPIO_MODE_IN },
            },
            [SX1262_PIN_DIO1] = &(gpio_t){
                .pin = { .pin = 14 },
                .cfg = { .mode = GPIO_MODE_IN },
            },
        },
    },
    .freq_hz = 868000000,
    .power = 14,
    .tcxo = 1,
    .tcxo_voltage = SX1262_TCXO_1_8V,
    .dio2_rf_switch = 1,
    .mod = { .sf = SX1262_SF7, .bw = SX1262_BW_125, .cr = SX1262_CR_4_5, .ldro = SX1262_LDRO_OFF },
    .pkt = { .preamble_len = 8, .header_type = SX1262_HEADER_EXPLICIT, .crc = SX1262_CRC_ON, .invert_iq = SX1262_IQ_STANDARD },
};

unsigned brightness = 9;

extern font_t font_5x7;

const lcd_font_cfg_t fcfg = {
    .font = &font_5x7,
    .gaps = { .x = 4, .y = 4 },
    .scale = 2
};

color_scheme_t cs_val  = { .fg = 0xFFFF, .bg = 0x0000 };
color_scheme_t cs_help = { .fg = 0x8410, .bg = 0x0000 };
color_scheme_t cs_err  = { .fg = 0xF800, .bg = 0x0000 };

tptr_t tp;
ping_stats_t ping_status = {};

/* ── helpers ─────────────────────────────────────────────────────────── */

#define COL_W  6   /* ширина числового поля в символах */

static void print_uint(tptr_t * t, unsigned val, color_scheme_t cs)
{
    char buf[COL_W + 1];
    dec_to_str_right_aligned(val, buf, COL_W, 0);
    lcd_color_tptr_print(t, buf, cs, COL_W);
}

static void print_int(tptr_t * t, int val, color_scheme_t cs)
{
    char buf[COL_W + 1];

    if (val < 0) {
        buf[0] = '-';
        dec_to_str_right_aligned((unsigned)(-val), buf + 1, COL_W - 1, 0);
    } else {
        buf[0] = ' ';
        dec_to_str_right_aligned((unsigned)val, buf + 1, COL_W - 1, 0);
    }
    lcd_color_tptr_print(t, buf, cs, COL_W);
}

/* ── отрисовка ───────────────────────────────────────────────────────── */

/*
 * строка 0:  [t] send. ping seq:         1
 * строка 1:              timeout / (пусто)
 * строка 2:  (пустая)
 * строка 3:  --- remote ---
 * строка 4:     seq:      1     cnt:      0
 * строка 5:    rssi:    -52     snr:     11
 * строка 6:  (пустая)
 * строка 7:  --- received ---
 * строка 8:    rssi:    -48     snr:      8
 */

static void draw_header(void)
{
    lcd_select();
    text_ptr_set_char_pos(&tp, (xy_t){ .x = 0, .y = 0 });
    lcd_color_tptr_print(&tp, "[t] send. ping seq:", cs_help, 19);
    print_uint(&tp, ping_status.seq, cs_val);

    text_ptr_set_char_pos(&tp, (xy_t){ .x = 0, .y = 1 });
    lcd_color_tptr_print(&tp, "                   ", cs_val, 19);
    lcd_unselect();
}

static void draw_timeout(void)
{
    lcd_select();
    text_ptr_set_char_pos(&tp, (xy_t){ .x = 12, .y = 1 });
    lcd_color_tptr_print(&tp, "timeout", cs_err, 7);
    lcd_unselect();
}

static void draw_remote(pong_state_t * p)
{
    lcd_select();
    text_ptr_set_char_pos(&tp, (xy_t){ .x = 0, .y = 3 });
    lcd_color_tptr_print(&tp, "--- remote ---", cs_help, 14);

    text_ptr_set_char_pos(&tp, (xy_t){ .x = 0, .y = 4 });
    lcd_color_tptr_print(&tp, "   seq:", cs_val, 7);
    print_uint(&tp, p->seq, cs_val);
    lcd_color_tptr_print(&tp, "     cnt:", cs_val, 9);
    print_uint(&tp, p->cnt, cs_val);

    text_ptr_set_char_pos(&tp, (xy_t){ .x = 0, .y = 5 });
    lcd_color_tptr_print(&tp, "  rssi:", cs_val, 7);
    print_int(&tp, p->q.rssi, cs_val);
    lcd_color_tptr_print(&tp, "     snr:", cs_val, 9);
    print_int(&tp, p->q.snr, cs_val);
    lcd_unselect();
}

static void draw_received(rssi_snr_t * q)
{
    lcd_select();
    text_ptr_set_char_pos(&tp, (xy_t){ .x = 0, .y = 7 });
    lcd_color_tptr_print(&tp, "--- received ---", cs_help, 16);

    text_ptr_set_char_pos(&tp, (xy_t){ .x = 0, .y = 8 });
    lcd_color_tptr_print(&tp, "  rssi:", cs_val, 7);
    print_int(&tp, q->rssi, cs_val);
    lcd_color_tptr_print(&tp, "     snr:", cs_val, 9);
    print_int(&tp, q->snr, cs_val);
    lcd_unselect();
}

static void draw_result(ping_stats_t * s)
{
    text_ptr_set_char_pos(&tp, (xy_t){ .x = 0, .y = 1 });
    lcd_color_tptr_print(&tp, "                   ", cs_val, 19);

    draw_remote(&s->pong);
    draw_received(&s->q);
}

static form_t display_lcd_cfg_form(lcd_cfg_t * cfg)
{
    return (form_t){
        .p = { .x = 0, .y = 0 },
        .s = { .w = cfg->gcfg.width, .h = cfg->gcfg.height }
    };
}

static void ui_init(form_t * f)
{
    tp = text_ptr_create((tf_t){
        .fcfg = &fcfg,
        .pos  = f->p,
        .lim  = fcfg_text_char_places(&fcfg, f->s)
    });

    lcd_select();
    lcd_rect(0, 0, f->s.w, f->s.h, 0x0000);
    lcd_unselect();

    draw_header();
    draw_remote(&ping_status.pong);
    draw_received(&ping_status.q);
}

/* ── клавиатура ──────────────────────────────────────────────────────── */

void kbd_change_handler(unsigned num, unsigned state)
{
    dp("key : "); dpd(num, 2); dp(" = "); dpd(state, 1); dn();


    if (state != 1) {
        return;
    }

    num--;

    if (num == TLORA_KBD_KEY_FN) {
        bq25896_power_off();
    }

    if (num != TLORA_KBD_KEY_T) {
        return;
    }

    ping_status.seq++;
    draw_header();

    dp("sent: #"); dpd(ping_status.seq, 6); dn();

    if (do_ping(&ping_status, PONG_TIMEOUT_MS)) {
        dp("remote:   "); dp_lora_signal(&ping_status.pong.q); dp("  seq: "); dpd(ping_status.pong.seq, 6); dp("  cnt: "); dpd(ping_status.pong.cnt, 6); dn();
        dp("received: "); dp_lora_signal(&ping_status.q); dn();
        draw_result(&ping_status);
    } else {
        dpn("timeout");
        draw_timeout();
    }
    dn();
}

/* ── main ────────────────────────────────────────────────────────────── */

int main(void)
{
    dpn("tlora_lcd_ping start");

    init_gpio(&kbd_irq_line);
    init_i2c(&i2c_bus_cfg);
    init_tca8418();

    init_lcd_hw(&lcd_cfg);
    lcd_bl(16 - brightness);
    init_lcd(&lcd_cfg);

    form_t lcdf = display_lcd_cfg_form(&lcd_cfg);
    ui_init(&lcdf);

    init_xl9555_gpio(&lora_pwr);
    xl9555_gpio_set(&lora_pwr, 1);
    dpn("lora power on");
    delay_ms(100);

    if (!lora_init(&lora_cfg)) {
        dpn("lora FAILED");
        while (1) {};
    }

    dpn("lora ready");

    while (1) {
        if (gpio_get_state(&kbd_irq_line) == 0) {
            tca8418_poll_kp_fifo(kbd_change_handler);
        }
    }
}
