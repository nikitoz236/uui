/*
 * Отладочный вывод через встроенный USB Serial/JTAG контроллер ESP32-S3.
 *
 * ESP32-S3 имеет аппаратный USB Serial/JTAG (не требует внешнего USB-UART
 * моста). Endpoint 1 (EP1) — CDC ACM IN: данные пишутся побайтно в FIFO
 * через rdwr_byte, отправка пакета хосту — wr_done = 1.
 *
 * Используется как бэкенд dp()/dpx()/dpn() во всех ESP32-S3 тестах.
 *
 * ── Проблема ────────────────────────────────────────────────────────────
 *
 * FIFO вмещает 64 байта. dp()/dpd()/dpx() — отдельные вызовы
 * __debug_usart_tx_data(), каждый с несколькими байтами. Одна строка
 * вывода = 5-10 вызовов. Если делать wr_done на каждый вызов —
 * генерируем 5-10 USB пакетов на строку. Хост читает 1 пакет за USB
 * frame (~1 мс), второй вызов видит FIFO full → данные теряются.
 *
 * ── Решение: wr_done только при переполнении или '\n' ───────────────
 *
 * Несколько dp()-вызовов накапливают байты в FIFO. wr_done делается:
 *   1) FIFO полон (64 байта) — flush mid-line, busy-wait до ~1 мс
 *   2) Встретился '\n' — конец строки, финальный flush
 *
 * Для строки < 64 байт — один USB пакет на строку.
 * Для строки > 64 байт — два+ пакета: flush на 64-м байте + flush на \n.
 *
 * ── Детект хоста ────────────────────────────────────────────────────────
 *
 * static host_active — софтверный флаг. При FIFO full:
 *   host_active=1: busy-wait до 50000 итераций (~1 мс). Если таймаут —
 *                  хост не читает, host_active=0, return.
 *   host_active=0: проверяем FIFO и IN token IRQ один раз (~100 нс).
 *                  Если хост вернулся — host_active=1, продолжаем.
 *                  Иначе — return.
 *
 * IN token IRQ (in_token_rec_in_ep1): аппаратный бит, хост прислал
 * IN token на EP1. Используется только в host_active=0 для быстрого
 * обнаружения подключения хоста. Нельзя использовать как gate на входе
 * каждого вызова — между dp() внутри строки проходят микросекунды,
 * хост не успеет прислать новый token после очистки бита.
 *
 * ── Поведение без кабеля ────────────────────────────────────────────────
 *
 * 1) После старта FIFO пуст, host_active=1.
 * 2) Первые dp()-вызовы пишут в FIFO (< 64 байт — без wr_done).
 * 3) На 64-м байте или при следующем вызове когда FIFO full:
 *    wr_done → busy-wait → таймаут ~1 мс → host_active=0 → return.
 *    FIFO остаётся забит 64 байтами, \n не записан.
 * 4) Все дальнейшие вызовы: FIFO full → wr_done → одна проверка →
 *    full → return. ~100 нс на вызов.
 * 5) При подключении монитора: хост читает стухший пакет, FIFO
 *    освобождается. Следующий dp() видит FIFO free → host_active=1.
 *    Первая строка может быть склеена с остатком, дальше всё чисто.
 */

#include "soc/usb_serial_jtag_struct.h"

void dbg_usb_cdc_acm_tx(const char * s, unsigned len)
{
    static unsigned host_active = 1;
    unsigned flush = 0;

    while (len--) {
        if (!USB_SERIAL_JTAG.ep1_conf.serial_in_ep_data_free) {
            USB_SERIAL_JTAG.ep1_conf.wr_done = 1;

            if (!host_active) {
                if (USB_SERIAL_JTAG.ep1_conf.serial_in_ep_data_free) {
                    host_active = 1;
                } else if (USB_SERIAL_JTAG.int_raw.in_token_rec_in_ep1_int_raw) {
                    USB_SERIAL_JTAG.int_clr.in_token_rec_in_ep1_int_clr = 1;
                    host_active = 1;
                } else {
                    return;
                }
            }

            unsigned t = 50000;
            while (!USB_SERIAL_JTAG.ep1_conf.serial_in_ep_data_free) {
                if (--t == 0) {
                    host_active = 0;
                    return;
                }
            }
        }

        char c = *s++;
        USB_SERIAL_JTAG.ep1.rdwr_byte = c;

        if (c == '\n') {
            flush = 1;
        }
    }

    if (flush) {
        USB_SERIAL_JTAG.ep1_conf.wr_done = 1;
    }
}
