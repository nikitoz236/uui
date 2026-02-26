#include "lora.h"

/* ── калибровка образа по частотному диапазону ─────────────────────────── */

static void calibrate_image(const sx1262_cfg_t * chip, uint32_t freq_hz)
{
    uint8_t f1, f2;
    if (freq_hz >= 902000000) {
        f1 = 0xE1;
        f2 = 0xE9;
    } else if (freq_hz >= 863000000) {
        f1 = 0xD7;
        f2 = 0xDB;
    } else if (freq_hz >= 779000000) {
        f1 = 0xC1;
        f2 = 0xC5;
    } else if (freq_hz >= 470000000) {
        f1 = 0x75;
        f2 = 0x81;
    } else {
        f1 = 0x6B;
        f2 = 0x6F;
    }
    sx1262_calibrate_image(chip, f1, f2);
}

/* ── инициализация ────────────────────────────────────────────────────── */

void lora_init(const lora_cfg_t * cfg)
{
    if (cfg->tcxo_voltage != LORA_TCXO_NONE) {
        sx1262_set_tcxo_mode(&cfg->chip, cfg->tcxo_voltage, 320);
    }
    sx1262_calibrate(&cfg->chip, 0x7F);
    calibrate_image(&cfg->chip, cfg->freq_hz);

    if (cfg->dio2_rf_switch) {
        sx1262_set_dio2_rf_switch_ctrl(&cfg->chip, 1);
    }

    sx1262_set_packet_type(&cfg->chip);
    sx1262_set_rf_frequency(&cfg->chip, cfg->freq_hz);

    /* PA: max capability (duty=0x04, hp_max=0x07), ramp 200us */
    sx1262_set_pa_config(&cfg->chip, 0x04, 0x07);
    sx1262_set_tx_params(&cfg->chip, cfg->power, 0x04);

    sx1262_set_mod_params(&cfg->chip, cfg->mod.sf, cfg->mod.bw, cfg->mod.cr, cfg->mod.ldro);
    sx1262_set_packet_params(&cfg->chip, cfg->pkt.preamble_len, cfg->pkt.header_type, 0, cfg->pkt.crc, cfg->pkt.invert_iq);
    sx1262_set_buffer_base_addr(&cfg->chip, 0, 0);
    lora_irq_t mask = { .tx_done = 1, .rx_done = 1, .crc_err = 1, .timeout = 1 };
    sx1262_set_irq_mask(&cfg->chip, mask.raw);
}

/* ── передача (блокирующая) ───────────────────────────────────────────── */

unsigned lora_send(const lora_cfg_t * cfg, const uint8_t * data, uint8_t len)
{
    sx1262_set_packet_params(&cfg->chip, cfg->pkt.preamble_len, cfg->pkt.header_type, len, cfg->pkt.crc, cfg->pkt.invert_iq);
    sx1262_clear_irq_status(&cfg->chip, 0xFFFF);
    sx1262_write_buffer(&cfg->chip, 0, data, len);
    sx1262_set_tx(&cfg->chip, 640000); /* timeout ~10s */

    while (!gpio_get_state(cfg->chip.pin[SX1262_PIN_DIO1])) {};

    lora_irq_t irq = { .raw = sx1262_get_irq_status(&cfg->chip) };
    sx1262_clear_irq_status(&cfg->chip, irq.raw);

    return irq.tx_done;
}

/* ── приём ─────────────────────────────────────────────────────────────── */

void lora_rx_start(const lora_cfg_t * cfg)
{
    sx1262_clear_irq_status(&cfg->chip, 0xFFFF);
    sx1262_set_rx(&cfg->chip, SX1262_RX_CONTINUOUS);
}

int lora_rx_read(const lora_cfg_t * cfg, uint8_t * data, uint8_t max_len)
{
    if (!gpio_get_state(cfg->chip.pin[SX1262_PIN_DIO1])) {
        return 0;
    }

    lora_irq_t irq = { .raw = sx1262_get_irq_status(&cfg->chip) };
    sx1262_clear_irq_status(&cfg->chip, irq.raw);

    if (irq.crc_err) {
        return -1;
    }

    if (!irq.rx_done) {
        return 0;
    }

    uint8_t payload_len, rx_offset;
    sx1262_get_rx_buffer_status(&cfg->chip, &payload_len, &rx_offset);

    if (payload_len > max_len) {
        payload_len = max_len;
    }

    sx1262_read_buffer(&cfg->chip, rx_offset, data, payload_len);
    return payload_len;
}
