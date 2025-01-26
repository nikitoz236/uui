#include "sd_card.h"
#include "dp.h"
#include "buf_endian.h"

#define DATA_TOKEN                      0xFE

static uint8_t send_cmd(sd_cfg_t * cfg, uint8_t cmd, uint32_t data, uint8_t crc)
{
    // можно иметь буффер команды и отправлять его по dma

    uint8_t cmd_buf[6];
    cmd_buf[0] = 0x40 + cmd;
    cmd_buf[5] = crc;
    u32_to_be_buf8(&cmd_buf[1], data);

    for (unsigned i = 0; i < 6; i++) {
        spi_write_8(cfg->spi_dev.spi, cmd_buf[i]);
    }

    dp("  send cmd to sd: "); dpxd(cmd_buf, 1, 6); dn();

    unsigned count = 0;
    while (count < 1000) {
        uint8_t answ = spi_exchange_8(cfg->spi_dev.spi, 0xFF);
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

    resp = send_cmd(cfg, 0, 0, 0x95);
    if (resp == 0xFF) {
        return SD_TYPE_NOT_INITIALISATED;
    }

    resp = send_cmd(cfg, 8, 0x1AA, 0x87);
    if (resp == 0xFF) {
        return SD_TYPE_NOT_INITIALISATED;
    }

    if (resp & (1 << 2)) {
        // illegal answer = SD V1.x
        // some initialisation code for V1.x cards
        return SD_TYPE_MMC;
    }

    // resp = spi_exchange_8(cfg->spi_dev.spi, 0xFF);
    // dp("  send to sd FF, just interest, resp: "); dpx(resp, 1); dn();
    // resp = spi_exchange_8(cfg->spi_dev.spi, 0xFF);
    // dp("  send to sd FF, just interest, resp: "); dpx(resp, 1); dn();
    // resp = spi_exchange_8(cfg->spi_dev.spi, 0xFF);
    // dp("  send to sd FF, just interest, resp: "); dpx(resp, 1); dn();
    // resp = spi_exchange_8(cfg->spi_dev.spi, 0xFF);
    // dp("  send to sd FF, just interest, resp: "); dpx(resp, 1); dn();
    // resp = spi_exchange_8(cfg->spi_dev.spi, 0xFF);
    // dp("  send to sd FF, just interest, resp: "); dpx(resp, 1); dn();
    // resp = spi_exchange_8(cfg->spi_dev.spi, 0xFF);
    // dp("  send to sd FF, just interest, resp: "); dpx(resp, 1); dn();
    // resp = spi_exchange_8(cfg->spi_dev.spi, 0xFF);
    // dp("  send to sd FF, just interest, resp: "); dpx(resp, 1); dn();
    // resp = spi_exchange_8(cfg->spi_dev.spi, 0xFF);
    // dp("  send to sd FF, just interest, resp: "); dpx(resp, 1); dn();


    spi_write_8(cfg->spi_dev.spi, 0xFF);
    spi_write_8(cfg->spi_dev.spi, 0xFF);
    spi_write_8(cfg->spi_dev.spi, 0xFF);
    spi_write_8(cfg->spi_dev.spi, 0xFF);

    //read OCR
    dpn("SD read OCR");
    resp = send_cmd(cfg, 58, 0, 0);
    if (resp == 0xFF) {
        return SD_TYPE_NOT_INITIALISATED;
    }

    uint8_t type = spi_exchange_8(cfg->spi_dev.spi, 0xFF);
    dp("  read sd type, send to sd FF, resp: "); dpx(type, 1); dn();

    spi_write_8(cfg->spi_dev.spi, 0xFF);
    spi_write_8(cfg->spi_dev.spi, 0xFF);
    spi_write_8(cfg->spi_dev.spi, 0xFF);

    dpn("SD send init cmd");
    unsigned count = 0;
    while (count < 1000) {
        send_cmd(cfg, 55, 0, 0);
        resp = send_cmd(cfg, 41, 0x40000000, 0xFF);
        if (resp == 0x00) {
            dp("SD card initialized in "); dpd(count, 3); dp(" tries"); dn();
            break;
        }
        count++;
    }

    // resp = spi_exchange_8(cfg->spi_dev.spi, 0xFF);
    // dp("  send to sd FF, just interest, resp: "); dpx(resp, 1); dn();

    if (count >= 1000) {
        return SD_TYPE_NOT_INITIALISATED;
    }

    if (type & (1 << 6)) {
        return SD_TYPE_SDHC;
    }
    return SD_TYPE_SDSC;
}

static uint16_t read_data(sd_cfg_t * cfg, uint8_t * buffer, unsigned len)
{
    unsigned count = 0;
    while (count < 1000) {
        uint8_t answ = spi_exchange_8(cfg->spi_dev.spi, 0xFF);
        if (answ == DATA_TOKEN) {
            dpn("read FE recieved");
            break;
        } else if ((answ & 0xE0) == 0) {
            dpn("Data read error token: "); dpx(answ, 1); dn();
            return 0;
        }
        count++;
    }

    for (unsigned i = 0; i < len; i++) {
        buffer[i] = spi_exchange_8(cfg->spi_dev.spi, 0xFF);
    }

    uint16_t crc = spi_exchange_8(cfg->spi_dev.spi, 0xFF) << 8;
    crc += spi_exchange_8(cfg->spi_dev.spi, 0xFF);
    return crc;
}

void sd_read_cid(sd_cfg_t * cfg, struct sd_cid * cid)
{
    send_cmd(cfg, 10, 0, 0);
    read_data(cfg, (uint8_t*)cid, sizeof(struct sd_cid));
}

void sd_read_sector(sd_cfg_t * cfg, uint32_t sector_addr, uint8_t * buf)
{
    send_cmd(cfg, 17, sector_addr, 0);
    read_data(cfg, buf, SD_SECTOR_SIZE);
}

uint8_t sd_write_sector(sd_cfg_t * cfg, uint32_t sector_addr, const uint8_t * buf)
{
    send_cmd(cfg, 24, sector_addr, 0);

    spi_write_8(cfg->spi_dev.spi, 0xFF);
    spi_write_8(cfg->spi_dev.spi, DATA_TOKEN);
    for (unsigned i = 0; i < SD_SECTOR_SIZE; i++) {
        spi_write_8(cfg->spi_dev.spi, buf[i]);
    }
    spi_write_8(cfg->spi_dev.spi, 0xFF);    // crc - not used
    spi_write_8(cfg->spi_dev.spi, 0xFF);

    uint8_t status = 0xFF;
    /*
        незнаю что там происходит ниже
        вроде как карта должна ответить 0x05, после этого возвращать нули пока будет занята. однако прилетает странное

        FF 00 00 00 00 00 00 00 00 7F FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
        тоесть 5ки не было, сразу пошли нули, а потом вообще 7F
        в итоге просто ждем пока не будет 0xFF


    unsigned count = 0;
    while (count < 40) {
        status = spi_exchange_8(cfg->spi_dev.spi, 0xFF);
        if ((status & 0x11) == 1) {
            dp("write data sector: "); dpx(sector_addr, 4); dp(" status: "); dpx(status, 1); dp(" num try: "); dpd(count, 8); dn();
            break;
        }

        dpx(status, 1);
        dp(" ");
        // for(volatile unsigned i = 0; i < 10000000; i++) {};
        count++;
    }

    if (count >= 40) {
        dpn("\nwrite data sector timeout");
        return 0xFF;
    }

    */

    // wait card busy
    while (spi_exchange_8(cfg->spi_dev.spi, 0xFF) == 0x00) {};

    dpn("write data sector finished");

    return status;
}
