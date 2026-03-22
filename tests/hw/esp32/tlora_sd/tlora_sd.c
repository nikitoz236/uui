#include "esp32_gpio.h"
#include "esp32_spi.h"
#include "esp32_pwm.h"

#include "dbg_usb_cdc_acm.h"

#define DP_NOTABLE
#include "dp.h"

#include "delay_blocking.h"
#include "sd_card.h"
#include "xl9555.h"
#include "esp32_i2c.h"

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

const spi_cfg_t spi = {
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

const sd_cfg_t sd = {
    .spi_dev = {
        .spi = &spi,
        .cs_pin = &(gpio_t){
            .pin = { .pin = 21 },
            .cfg = { .mode = GPIO_MODE_OUT }
        }
    }
};

const i2c_cfg_t i2c_bus_cfg = {
    .dev = &I2C0,
    .i2c_pclk = SYSTEM_I2C_EXT0_CLK_EN_S,
    .freq = 400000,
    .pin_list = &(gpio_list_t){
        .count = 2,
        .cfg = { .mode = GPIO_MODE_SIG_IO, .pu = 1, .od = 1 },
        .pin_list = {
            { .pin = 2, .signal = I2CEXT0_SCL_IN_IDX },
            { .pin = 3, .signal = I2CEXT0_SDA_IN_IDX }
        }
    }
};

const xl9555_gpio_t sd_pwr = {
    .dir = XL9555_DIR_OUT,
    .pin = 8 + 4
};

#include "fat32.h"

uint8_t sd_sector_buf[SD_SECTOR_SIZE];
unsigned loaded_sector = -1;

uint8_t * sector_load(unsigned sector)
{
    if (loaded_sector != sector) {
        loaded_sector = sector;
        // dp("read sector "); dpd(sector, 10);
        sd_read_sector(&sd, sector, sd_sector_buf);
        // dpn(" done");
    }
    return sd_sector_buf;
}

void read_fs(void)
{
    fat32_t fat;
    if (!init_fat32(&fat)) {
        dpn("init fat FAIL");
        return;
    }

    dpn("read fat .... OK");
    dp("  fat_offset[0]:       "); dpd(fat.fat_offset[0], 8); dn();
    dp("  fat_offset[1]:       "); dpd(fat.fat_offset[1], 8); dn();
    dp("  sectors_per_cluster: "); dpd(fat.sectors_per_cluster, 8); dn();
    dp("  root_dir_cl:         "); dpd(fat.root_dir_cl, 8); dn();
    dp("  sector_of_zero_cl:   "); dpd(fat.sector_of_zero_cl, 8); dn();

    // dpn("fat:");
    // sector_dump(fat.fat_offset[0], 2);

    dpn("root dir:");
    // sector_dump(sector_of_cluster(&fat, fat.root_dir_cl), 12);

    unsigned fr = 0;
    unsigned r;
    char buf[1024];
    while (r = dir_scan(&fat, fat.root_dir_cl, fr, buf, 512)) {
        dp("read dir entry num "); dpd(r, 2); dp(" name !!! : "); dp(buf); dn();
        fr += r;
    }

    dpn("done");
}


int main(void)
{
    dpn("t lora sd card spi test");

    init_spi(&spi);
    dpn("spi inited");

    init_i2c(&i2c_bus_cfg);
    dpn("[init] i2c ok");

    init_xl9555_gpio(&sd_pwr);
    xl9555_gpio_set(&sd_pwr, 0);
    delay_ms(50);
    xl9555_gpio_set(&sd_pwr, 1);
    delay_ms(100);
    dpn("[init] sd power on");

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

        read_fs();
    }

    while (1) {
    }
}
