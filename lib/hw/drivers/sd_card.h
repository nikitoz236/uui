#pragma once
#include <stdint.h>
#include "spi.h"

typedef const struct {
    spi_dev_cfg_t spi_dev;
    gpio_t * lock;
    gpio_t * detect;
} sd_cfg_t;

#define SD_SECTOR_SIZE          512

struct __attribute__((packed)) sd_cid {
    uint8_t mid;             // Manufacturer ID
    uint16_t oid;            // OEM/Application ID
    uint8_t prn[5];          // Product name
    uint8_t prv;             // Product revision
    uint32_t psn;            // Product serial number
    uint16_t mdt;            // Manufacturing date (12 bit)
    uint8_t crc;             // CRC7 checksum (7 bit, lsb - always 1 )
};

struct __attribute__((packed)) sd_csd {
    unsigned reserved_1 : 1;
    unsigned crc7 : 7;

    unsigned reserved_2 : 2;
    unsigned file_format : 2;
    unsigned tmp_write_protect : 1;
    unsigned perm_write_protect : 1;
    unsigned copy : 1;
    unsigned file_format_grp : 1;

    unsigned reserved_3 : 5;
    unsigned write_bl_partial : 1;
    unsigned write_bl_len : 4;
    unsigned r2w_factor : 3;
    unsigned reserved_4 : 2;
    unsigned wp_grp_enable : 1;

    uint8_t hz[6];

    // unsigned wb_grp_size : 7;
    // unsigned sector_size : 7;
    // unsigned erase_blk_en : 1;

    // union {
    //     struct __attribute__((packed)) {
    //         unsigned c_size_mult : 3;
    //         unsigned vdd_w_curr_max : 3;
    //         unsigned vdd_w_curr_min : 3;

    //         unsigned vdd_r_curr_max : 3;
    //         unsigned vdd_r_curr_min : 3;
    //         unsigned c_size : 12;
    //         unsigned reserved_5 : 2;
    //     } v1;
    //     struct __attribute__((packed)) {
    //         unsigned reserved_5 : 1;

    //         unsigned c_size : 22;
    //         unsigned reserved_6 : 6;
    //     } v2;
    // };

    // unsigned dsr_imp : 1;
    // unsigned read_blk_misalign : 1;
    // unsigned write_blk_misalign : 1;
    // unsigned read_bl_partial : 1;

    unsigned read_bl_len : 4;
    unsigned ccc : 12;

    unsigned tran_speed : 8;

    unsigned nsac : 8;

    unsigned taac : 8;

    unsigned reserved_6 : 6;
    unsigned csd_structure : 2;
};

struct __attribute__((packed)) sd_csd_v1 {
    unsigned reserved_1 : 1;
    unsigned crc7 : 7;

    unsigned reserved_2 : 2;
    unsigned file_format : 2;
    unsigned tmp_write_protect : 1;
    unsigned perm_write_protect : 1;
    unsigned copy : 1;
    unsigned file_format_grp : 1;

    unsigned reserved_3 : 5;
    unsigned write_bl_partial : 1;
    unsigned write_bl_len : 4;
    unsigned r2w_factor : 3;
    unsigned reserved_4 : 2;
    unsigned wp_grp_enable : 1;

    unsigned wb_grp_size : 7;
    unsigned sector_size : 7;
    unsigned erase_blk_en : 1;
    unsigned c_size_mult : 3;
    unsigned vdd_w_curr_max : 3;
    unsigned vdd_w_curr_min : 3;

    unsigned vdd_r_curr_max : 3;
    unsigned vdd_r_curr_min : 3;
    unsigned c_size : 12;
    unsigned reserved_5 : 2;
    unsigned dsr_imp : 1;
    unsigned read_blk_misalign : 1;
    unsigned write_blk_misalign : 1;
    unsigned read_bl_partial : 1;

    unsigned read_bl_len : 4;
    unsigned ccc : 12;

    unsigned tran_speed : 8;

    unsigned nsac : 8;

    unsigned taac : 8;

    unsigned reserved_6 : 6;
    unsigned scd_structure : 2;
};

struct __attribute__((packed)) sd_csd_v2 {
    unsigned reserved_1 : 1;
    unsigned crc7 : 7;

    unsigned reserved_2 : 2;
    unsigned file_format : 2;
    unsigned tmp_write_protect : 1;
    unsigned perm_write_protect : 1;
    unsigned copy : 1;
    unsigned file_format_grp : 1;

    unsigned reserved_3 : 5;
    unsigned write_bl_partial : 1;
    unsigned write_bl_len : 4;
    unsigned r2w_factor : 3;
    unsigned reserved_4 : 2;
    unsigned wp_grp_enable : 1;

    unsigned wb_grp_size : 7;
    unsigned sector_size : 7;
    unsigned erase_blk_en : 1;
    unsigned reserved_5 : 1;

    unsigned c_size : 22;
    unsigned reserved_6 : 6;
    unsigned dsr_imp : 1;
    unsigned read_blk_misalign : 1;
    unsigned write_blk_misalign : 1;
    unsigned read_bl_partial : 1;

    unsigned read_bl_len : 4;
    unsigned ccc : 12;
    unsigned tran_speed : 8;
    unsigned nsac : 8;
    unsigned taac : 8;
    unsigned reserved_7 : 6;
    unsigned scd_structure : 2;
};

enum sd_type {
    SD_TYPE_NOT_INITIALISATED = 0,
    SD_TYPE_MMC,
    SD_TYPE_SDSC,
    SD_TYPE_SDHC
};

enum sd_type init_sd(sd_cfg_t * cfg);
void sd_read_csd(sd_cfg_t * cfg, struct sd_csd * csd);
void sd_read_cid(sd_cfg_t * cfg, struct sd_cid * cid);
void sd_read_sector(sd_cfg_t * cfg, uint32_t sector_addr, uint8_t * buf);
uint8_t sd_write_sector(sd_cfg_t * cfg, uint32_t sector_addr, const uint8_t * buf);

// void sd_read_scd(uint8_t * buffer);
// void sd_printCID(u8 * buffer);
// u8 sd_write_sector(u8 * buffer, u32 sector_addr);

static inline unsigned sd_detect(sd_cfg_t * cfg)
{
    if (gpio_get_state(cfg->detect)) {
        return 0;
    }
    return 1;
}
