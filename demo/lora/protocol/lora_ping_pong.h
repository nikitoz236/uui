#pragma once

#include <stdint.h>

/*
 * Протокол ping-pong для тестирования LoRa канала.
 *
 * ping (ESP32 → STM32): голый uint32_t seq.
 * pong (STM32 → ESP32): структура с эхом seq, статистикой потерь
 * и параметрами сигнала на стороне приёмника.
 */

typedef struct __attribute__((packed)) {
    uint32_t rx_seq; /* seq из принятого ping */
    uint32_t rx_lost; /* потеряно с прошлого принятого ping (gap) */
    uint32_t rx_cnt; /* сколько ping принято всего */
    uint32_t tx_cnt; /* счётчик отправленных pong */
    uint8_t rssi; /* rssi_raw: RSSI = -(rssi/2) dBm */
    int8_t snr; /* snr_raw:  SNR = snr/4 dB */
} lora_pong_t; /* 18 байт */

/* ── rssi / snr ──────────────────────────────────────────────────── */

typedef struct {
    int8_t rssi; /* RSSI: -(raw/2) dBm */
    int8_t snr; /* SNR:   raw/4 dB */
} rssi_snr_t;

static inline rssi_snr_t rssi_snr_from_raw(uint8_t rssi, int8_t snr)
{
    rssi_snr_t rs = { .rssi = -(int8_t)(rssi / 2), .snr = (int8_t)(snr / 4) };
    return rs;
}

/* ── ping stats ──────────────────────────────────────────────────── */

typedef struct {
    uint32_t seq; /* текущий seq */
    uint32_t cnt; /* сколько pong приняли */
    uint32_t lost; /* сколько pong потеряли (timeout) */
} ping_stat_t;

typedef struct {
    ping_stat_t stat; /* локальные счётчики */
    rssi_snr_t rem; /* удалённая сторона */
    rssi_snr_t loc; /* локальная сторона */
    lora_pong_t pong; /* принятый pong пакет */
} ping_stats_t;

/* ── pong state ──────────────────────────────────────────────────── */

typedef struct {
    uint32_t rx_cnt;
    uint32_t tx_cnt;
    uint32_t prev_seq;
    rssi_snr_t loc; /* rssi/snr принятого ping */
} pong_state_t;

/* ── protocol functions ──────────────────────────────────────────── */

/*
 * do_ping: отправить seq, ждать pong до timeout_ms.
 * Возвращает 1 если pong получен (st заполнена), 0 при таймауте.
 */
unsigned do_ping(uint32_t seq, ping_stats_t * st, unsigned timeout_ms);

/*
 * do_pong: проверить есть ли принятый ping, если да — отправить pong.
 * Возвращает: 1 = pong отправлен, 0 = нет данных / ошибка.
 * Состояние хранит внутри (static). Печатает debug через dp.
 */
unsigned do_pong(void);
