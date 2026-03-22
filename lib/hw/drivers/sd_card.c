#include "sd_card.h"
#include "buf_endian.h"

#define DP_OFF
#define DP_NAME "sd card"
#include "dp.h"

const uint8_t data_token = 0xFE;

static void send_ff(sd_cfg_t * cfg, unsigned len)
{
    while (len--) {
        spi_write_8(cfg->spi_dev.spi, 0xFF);
    }
}

static uint8_t read(sd_cfg_t * cfg)
{
    return spi_exchange_8(cfg->spi_dev.spi, 0xFF);
}

static inline void write(sd_cfg_t * cfg, uint8_t * data, unsigned len)
{
    while (len--) {
        spi_write_8(cfg->spi_dev.spi, *data++);
    }
}

static uint8_t send_cmd(sd_cfg_t * cfg, uint8_t cmd, uint32_t data, uint8_t crc)
{
    // можно иметь буффер команды и отправлять его по dma

    uint8_t cmd_buf[6];
    cmd_buf[0] = 0x40 + cmd;
    cmd_buf[5] = crc;
    u32_to_be_buf8(&cmd_buf[1], data);

    dp("  send cmd to sd: "); dpxd(cmd_buf, 1, 6); dn();
    write(cfg, cmd_buf, 6);

    unsigned count = 0;
    while (count < 1000) {
        uint8_t answ = read(cfg);
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

    if (cfg->lock) {
        init_gpio(cfg->lock);
    }
    if (cfg->detect) {
        init_gpio(cfg->detect);
    }

    spi_dev_unselect(&cfg->spi_dev);
    send_ff(cfg, 10);
    spi_dev_select(&cfg->spi_dev);

    resp = send_cmd(cfg, 0, 0, 0x95);
    if (resp == 0xFF) {
        return SD_TYPE_NOT_INITIALISATED;
    }

    resp = send_cmd(cfg, 8, 0x1AA, 0x87);
    if (resp == 0xFF) {
        return SD_TYPE_NOT_INITIALISATED;
    }

    /* CMD8 returns R7: 4 trailing bytes must be consumed regardless */
    uint8_t r7[4];
    for (int i = 0; i < 4; i++) {
        r7[i] = read(cfg);
    }
    dp("CMD8 R7: "); dpxd(r7, 1, 4); dn();

    if (resp & (1 << 2)) {
        // illegal answer = SD V1.x
        // some initialisation code for V1.x cards
        return SD_TYPE_MMC;
    }

    send_ff(cfg, 4);

    //read OCR
    dpn("SD read OCR");
    resp = send_cmd(cfg, 58, 0, 0);
    if (resp == 0xFF) {
        return SD_TYPE_NOT_INITIALISATED;
    }

    uint8_t type = read(cfg);
    dp("  read sd type, send to sd FF, resp: "); dpx(type, 1); dn();

    send_ff(cfg, 3);

    dpn("SD send init cmd");
    unsigned count = 0;
    while (count < 1000) {
        send_cmd(cfg, 55, 0, 0);
        send_ff(cfg, 1);
        resp = send_cmd(cfg, 41, 0x40000000, 0xFF);
        /* drain R1b busy signal and trailing junk until MISO is idle */
        unsigned idle = 0;
        while (idle < 2) {
            if (read(cfg) == 0xFF) {
                idle++;
            } else {
                idle = 0;
            }
        }
        if (resp == 0x00) {
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

static uint16_t read_data(sd_cfg_t * cfg, uint8_t * buffer, unsigned len)
{
    unsigned count = 0;
    while (count < 1000) {
        uint8_t answ = read(cfg);
        if (answ == data_token) {
            dpn("read FE recieved");
            break;
        } else if ((answ & 0xE0) == 0) {
            dpn("Data read error token: "); dpx(answ, 1); dn();
            return 0;
        }
        count++;
    }

    for (unsigned i = 0; i < len; i++) {
        buffer[i] = read(cfg);
    }

    uint16_t crc = read(cfg) << 8;
    crc += read(cfg);
    return crc;
}

void sd_read_cid(sd_cfg_t * cfg, struct sd_cid * cid)
{
    send_cmd(cfg, 10, 0, 0);
    read_data(cfg, (uint8_t*)cid, sizeof(struct sd_cid));
}

void sd_read_csd(sd_cfg_t * cfg, struct sd_csd * csd)
{
    send_cmd(cfg, 9, 0, 0);
    read_data(cfg, (uint8_t*)csd, sizeof(struct sd_csd));
}

void sd_read_sector(sd_cfg_t * cfg, uint32_t sector_addr, uint8_t * buf)
{
    send_cmd(cfg, 17, sector_addr, 0);
    read_data(cfg, buf, SD_SECTOR_SIZE);
}

uint8_t sd_write_sector(sd_cfg_t * cfg, uint32_t sector_addr, const uint8_t * buf)
{
    send_cmd(cfg, 24, sector_addr, 0);

    send_ff(cfg, 1);
    write(cfg, &data_token, 1);
    write(cfg, buf, SD_SECTOR_SIZE);
    send_ff(cfg, 2);    // crc - not used

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
    while (read(cfg) == 0x00) {};

    dpn("write data sector finished");

    return status;
}
