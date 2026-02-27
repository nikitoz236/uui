/*
 * Отладочный вывод через встроенный USB Serial/JTAG контроллер ESP32-S3.
 *
 * ESP32-S3 имеет аппаратный USB Serial/JTAG (не требует внешнего USB-UART
 * моста). Endpoint 1 (EP1) — CDC ACM IN: данные пишутся побайтно в FIFO
 * через rdwr_byte, отправка пакета хосту — wr_done = 1.
 *
 * FIFO вмещает 64 байта. Если FIFO заполнен (serial_in_ep_data_free == 0),
 * отправляем что успели и выходим — данные теряются, но без задержки.
 * Когда монитор не подключён, вызов стоит ~100 нс (одна проверка регистра).
 *
 * Используется как бэкенд dp()/dpx()/dpn() во всех ESP32-S3 тестах.
 */

#include "soc/usb_serial_jtag_struct.h"

void dbg_usb_cdc_acm_tx(const char * s, unsigned len)
{
    while (len--) {
        if (!USB_SERIAL_JTAG.ep1_conf.serial_in_ep_data_free) {
            USB_SERIAL_JTAG.ep1_conf.wr_done = 1;
            return;
        }
        USB_SERIAL_JTAG.ep1.rdwr_byte = *s++;
    }
    USB_SERIAL_JTAG.ep1_conf.wr_done = 1;
}
