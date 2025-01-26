#include "sd_card.h"
#include "dp.h"
#include "buf_endian.h"

#define DATA_TOKEN                      0xFE

static uint8_t send_cmd(spi_cfg_t * cfg, uint8_t cmd, uint32_t data, uint8_t crc)
{
    // можно иметь буффер команды и отправлять его по dma

    uint8_t cmd_buf[6];
    cmd_buf[0] = 0x40 + cmd;
    cmd_buf[5] = crc;
    u32_to_be_buf8(&cmd_buf[1], data);

    for (unsigned i = 0; i < 6; i++) {
        spi_write_8(cfg, cmd_buf[i]);
    }

    unsigned count = 0;
    while (count < 1000) {
        uint8_t answ = spi_exchange_8(cfg, 0xFF);
        if ((answ & 0x80) == 0) {
            dp("R1 answer: "); dpx(answ, 1); dp(" num try: "); dpd(count, 3); dn();
            return answ;
        }
        count++;
    }
    dpn("answer R1 not recieved");
    return 0xFF;
}

enum sd_type init_sd(sd_cfg_t * cfg)
{
    uint8_t resp;
    init_spi_dev(&cfg->spi_dev);
    init_spi(cfg->spi_dev.spi);

    init_gpio(cfg->lock);
    init_gpio(cfg->detect);

    spi_dev_unselect(&cfg->spi_dev);
    for (int i = 0; i < 10; i++) {
        spi_write_8(cfg->spi_dev.spi, 0xFF);
    }
    spi_dev_select(&cfg->spi_dev);

    send_cmd(cfg->spi_dev.spi, 0, 0, 0x95);
    resp = wait_answer_r1(cfg->spi_dev.spi);
    if (resp == 0xFF) {
        return SD_TYPE_NOT_INITIALISATED;
    }

    send_cmd(cfg->spi_dev.spi, 8, 0x1AA, 0x87);
    resp = wait_answer_r1(cfg->spi_dev.spi);
    if (resp == 0xFF) {
        return SD_TYPE_NOT_INITIALISATED;
    }

    if (resp & (1 << 2)) {
        // illegal answer = SD V1.x
        // some initialisation code for V1.x cards
        return SD_TYPE_MMC;
    }

    spi_write_8(cfg->spi_dev.spi, 0xFF);
    spi_write_8(cfg->spi_dev.spi, 0xFF);
    spi_write_8(cfg->spi_dev.spi, 0xFF);
    spi_write_8(cfg->spi_dev.spi, 0xFF);

    //read OCR
    dpn("SD read OCR");
    send_cmd(cfg->spi_dev.spi, 58, 0, 0);
    resp = wait_answer_r1(cfg->spi_dev.spi);
    if (resp == 0xFF) {
        return SD_TYPE_NOT_INITIALISATED;
    }

    uint8_t type = spi_exchange_8(cfg->spi_dev.spi, 0xFF);

    spi_write_8(cfg->spi_dev.spi, 0xFF);
    spi_write_8(cfg->spi_dev.spi, 0xFF);
    spi_write_8(cfg->spi_dev.spi, 0xFF);

    dpn("SD send init cmd");
    unsigned count = 0;
    while (count < 1000) {
        send_cmd(cfg->spi_dev.spi, 55, 0, 0);
        wait_answer_r1(cfg->spi_dev.spi);
        send_cmd(cfg->spi_dev.spi, 41, 0x40000000, 0xFF);
        if (wait_answer_r1(cfg->spi_dev.spi) == 0x00) {
            dp("SD card initialized in "); dpd(count, 3); dp(" tries"); dn();
            break;
        }
        count++;
    }
    if (count >= 1000) {
        return SD_TYPE_NOT_INITIALISATED;
    }

    if (type & (1 << 6)) {
        return SD_TYPE_SDHC;
    }
    return SD_TYPE_SDSC;
}
