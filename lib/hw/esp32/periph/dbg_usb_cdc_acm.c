#include "soc/usb_serial_jtag_struct.h"

void dbg_usb_cdc_acm_tx(const char * s, unsigned len)
{
    while (len--) {
        while (USB_SERIAL_JTAG.ep1_conf.serial_in_ep_data_free == 0) {};
        USB_SERIAL_JTAG.ep1.rdwr_byte = *s++;
    }
    USB_SERIAL_JTAG.ep1_conf.wr_done = 1;
}
