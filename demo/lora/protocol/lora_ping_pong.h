#pragma once

#include <stdint.h>

/*
 * Протокол ping-pong для тестирования LoRa канала.
 *
 * ping (ESP32 → STM32): голый uint32_t seq.
 * pong (STM32 → ESP32): структура с эхом seq, статистикой потерь
 * и параметрами сигнала на стороне приёмника.
 */

/* ── rssi / snr ──────────────────────────────────────────────────── */

typedef struct {
    int8_t rssi; /* RSSI: -(raw/2) dBm */
    int8_t snr; /* SNR:   raw/4 dB */
} rssi_snr_t;

typedef struct {
    uint32_t cnt;
    uint32_t seq;
    rssi_snr_t q;
} pong_state_t;

typedef struct {
    uint32_t seq;
    pong_state_t pong;
    rssi_snr_t q;
} ping_stats_t;

void dp_lora_signal(rssi_snr_t * q);

/*
 * do_ping: отправить seq, ждать pong до timeout_ms.
 * Возвращает 1 если pong получен (st заполнена), 0 при таймауте.
 */
unsigned do_ping(ping_stats_t * state, unsigned timeout_ms);

/*
 * do_pong: проверить есть ли принятый ping, если да — отправить pong.
 * Возвращает: 1 = pong отправлен, 0 = нет данных / ошибка.
 * Состояние хранит внутри (static). Печатает debug через dp.
 */
unsigned do_responce(pong_state_t * state);
