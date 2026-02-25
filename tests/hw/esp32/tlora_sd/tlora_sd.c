#include "esp32_gpio.h"
#include "esp32_spi.h"
#include "esp32_pwm.h"

#include "dbg_usb_cdc_acm.h"

#define DP_NOTABLE
#include "dp.h"

#include "delay_blocking.h"
#include "sd_card.h"

/*

>----------Place the screen facing up---------------<
|---------------------------------------------------|
|     | SCL | SDA | MISO  | SCK  | TX | GND  |      |
|     | 5V  | CE  | GPIO9 | MOSI | RX | 3.3V |      |
|---------------------------------------------------|

*/

void __debug_usart_tx_data(const char * s, unsigned len)
{
    dbg_usb_cdc_acm_tx(s, len);
}

spi_cfg_t spi = {
    .spi = &GPSPI2,
    .pin_list = {
        [SPI_PIN_MOSI] = &(gpio_t){
            .cfg = { .mode = GPIO_MODE_SIG_OUT },
            .pin = { .pin = 34, .signal = FSPID_OUT_IDX },
        },
        [SPI_PIN_SCK] = &(gpio_t){
            .cfg = { .mode = GPIO_MODE_SIG_OUT },
            .pin = { .pin = 35, .signal = FSPICLK_OUT_IDX },
        },
        [SPI_PIN_MISO] = &(gpio_t){
            .cfg = { .mode = GPIO_MODE_SIG_IN },
            .pin = { .pin = 33, .signal = FSPIQ_IN_IDX },
        },
    },
};

sd_cfg_t sd = {
    .spi_dev = {
        .spi = &spi,
        .cs_pin = &(gpio_t){
            .pin = { .pin = 21 },
            .cfg = { .mode = GPIO_MODE_OUT }
        }
    }
};

int main(void)
{
    dpn("t lora sd card spi test");

    init_spi(&spi);
    dpn("spi inited");

    enum sd_type t = init_sd(&sd);
    dp("SD type: "); dpd(t, 1); dn();

    if (t == SD_TYPE_NOT_INITIALISATED) {
        dpn("SD init failed");
    } else {
        struct sd_cid cid;
        sd_read_cid(&sd, &cid);
        dp("CID MID: "); dpx(cid.mid, 1);
        dp(" OEM: "); dpxd((uint8_t*)&cid.oid, 1, 2);
        dp(" Name: "); dpxd(cid.prn, 1, 5);
        dn();

        struct sd_csd csd;
        sd_read_csd(&sd, &csd);
        dp("CSD ver: "); dpd(sd_csd_version(&csd), 1); dn();

        uint32_t size_mb = sd_csd_size_mb(&csd);
        dp("SD size: "); dpd(size_mb, 6); dp(" MB"); dn();
    }

    while (1) {
    }
}
