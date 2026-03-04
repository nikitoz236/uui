#include "lora.h"

/* ── калибровка образа по частотному диапазону ─────────────────────────── */

static void calibrate_image(const sx1262_cfg_t * chip, uint32_t freq_hz)
{
    static const struct {
        uint32_t freq_min;
        sx1262_calib_image_t img;
    } table[] = {
        { 902000000, { 0xE1, 0xE9 } },
        { 863000000, { 0xD7, 0xDB } },
        { 779000000, { 0xC1, 0xC5 } },
        { 470000000, { 0x75, 0x81 } },
        {         0, { 0x6B, 0x6F } },
    };
    for (unsigned i = 0; i < sizeof(table) / sizeof(table[0]); i++) {
        if (freq_hz >= table[i].freq_min) {
            sx1262_calibrate_image(chip, table[i].img);
            return;
        }
    }
}

/* ── инициализация ────────────────────────────────────────────────────── */

static const lora_cfg_t * lora;

unsigned lora_init(const lora_cfg_t * cfg)
{
    lora = cfg;

    sx1262_init_pins(&cfg->chip);
    sx1262_reset(&cfg->chip);

    if (!sx1262_check_connection(&cfg->chip)) {
        return 0;
    }

    if (cfg->tcxo) {
        sx1262_set_tcxo_mode(&cfg->chip, cfg->tcxo_voltage, 5);
    }
    sx1262_calibrate(&cfg->chip, (sx1262_reg_calib_t){ .raw = 0x7F });
    calibrate_image(&cfg->chip, cfg->freq_hz);

    if (cfg->dio2_rf_switch) {
        sx1262_set_dio2_rf_switch_ctrl(&cfg->chip, 1);
    }

    sx1262_set_packet_type(&cfg->chip, SX1262_PACKET_LORA);
    sx1262_set_rf_frequency(&cfg->chip, cfg->freq_hz);

    sx1262_set_pa_config(&cfg->chip, SX1262_PA_22DBM);
    sx1262_set_tx_params(&cfg->chip, cfg->power, SX1262_RAMP_200U);

    sx1262_set_mod_params(&cfg->chip, cfg->mod.sf, cfg->mod.bw, cfg->mod.cr, cfg->mod.ldro);
    sx1262_set_packet_params(&cfg->chip, cfg->pkt.preamble_len, cfg->pkt.header_type, 0, cfg->pkt.crc, cfg->pkt.invert_iq);
    sx1262_set_buffer_base_addr(&cfg->chip, 0, 0);
    sx1262_reg_irq_t mask = { .tx_done = 1, .rx_done = 1, .crc_err = 1, .timeout = 1 };
    sx1262_set_irq_mask(&cfg->chip, mask, cfg->irq_dio);
    return 1;
}

/* ── передача (блокирующая) ───────────────────────────────────────────── */

unsigned lora_send(const uint8_t * data, unsigned len)
{
    sx1262_set_packet_params(&lora->chip, lora->pkt.preamble_len, lora->pkt.header_type, len, lora->pkt.crc, lora->pkt.invert_iq);
    sx1262_clear_irq_status(&lora->chip, (sx1262_reg_irq_t){ .raw = 0xFFFF });
    sx1262_write_buffer(&lora->chip, 0, data, len);
    sx1262_set_tx(&lora->chip, 10000);

    while (!gpio_get_state(lora->chip.pin[SX1262_PIN_DIO1])) {};

    sx1262_reg_irq_t irq = sx1262_get_irq_status(&lora->chip);
    sx1262_clear_irq_status(&lora->chip, irq);

    return irq.tx_done;
}

/* ── приём ─────────────────────────────────────────────────────────────── */

void lora_rx_start(void)
{
    sx1262_set_packet_params(&lora->chip, lora->pkt.preamble_len, lora->pkt.header_type, 255, lora->pkt.crc, lora->pkt.invert_iq);
    sx1262_clear_irq_status(&lora->chip, (sx1262_reg_irq_t){ .raw = 0xFFFF });
    sx1262_set_rx(&lora->chip, SX1262_RX_CONTINUOUS);
}

unsigned lora_rx_read(uint8_t * data, unsigned max_len)
{
    if (!gpio_get_state(lora->chip.pin[SX1262_PIN_DIO1])) {
        return 0;
    }

    sx1262_reg_irq_t irq = sx1262_get_irq_status(&lora->chip);
    sx1262_clear_irq_status(&lora->chip, irq);

    if (irq.crc_err) {
        return 0;
    }

    if (!irq.rx_done) {
        return 0;
    }

    uint8_t payload_len, rx_offset;
    sx1262_get_rx_buffer_status(&lora->chip, &payload_len, &rx_offset);

    if (payload_len > max_len) {
        payload_len = max_len;
    }

    sx1262_read_buffer(&lora->chip, rx_offset, data, payload_len);
    return payload_len;
}

/* ── вспомогательные ──────────────────────────────────────────────────── */

void lora_standby(void)
{
    sx1262_set_standby(&lora->chip, SX1262_STANDBY_RC);
}

void lora_get_packet_status(uint8_t * rssi, int8_t * snr)
{
    sx1262_get_packet_status(&lora->chip, rssi, snr);
}
