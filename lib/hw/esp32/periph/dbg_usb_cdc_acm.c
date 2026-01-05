#include "soc/usb_serial_jtag_struct.h"

// volatile uint32_t usb_write_cnt = 0;

void dbg_usb_cdc_acm_tx(const char * s, unsigned len)
{
    // проверка что кабель подключен и порт открыт
    if (USB_SERIAL_JTAG.int_raw.in_token_rec_in_ep1_int_raw) {
        USB_SERIAL_JTAG.int_clr.in_token_rec_in_ep1_int_clr = 1;
    } else {
        // usb_write_cnt = 0;
        return;
    }

    // usb_write_cnt += len;

    while (len--) {
        while (USB_SERIAL_JTAG.ep1_conf.serial_in_ep_data_free == 0) {};
        USB_SERIAL_JTAG.ep1.rdwr_byte = *s++;
    }
    USB_SERIAL_JTAG.ep1_conf.wr_done = 1;
}
