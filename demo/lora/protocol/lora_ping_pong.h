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
    uint32_t rx_seq;        /* seq из принятого ping */
    uint32_t rx_lost;       /* потеряно с прошлого принятого ping (gap) */
    uint32_t rx_cnt;        /* сколько ping принято всего */
    uint32_t tx_cnt;        /* счётчик отправленных pong */
    uint8_t rssi;           /* rssi_raw: RSSI = -(rssi/2) dBm */
    int8_t snr;             /* snr_raw:  SNR = snr/4 dB */
} lora_pong_t;              /* 18 байт */
