#pragma once
#include <stdint.h>
#include <assert.h>
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
    uint8_t crc;             // CRC7 checksum (7 bit, lsb - always 1)
};

/*
    https://www.sdcard.org/downloads/pls/
    Part 1 Physical Layer Simplified Specification
    page 251
    5.3 CSD Register

        CSD register — 16 байт, big-endian (MSB первым с карты).
        Байты [6..11] различаются между v1 и v2 — вынесены в union.

        SD Spec раскладка:
            [0]      csd_structure[7:6], reserved[5:0]
            [1]      taac
            [2]      nsac
            [3]      tran_speed
            [4]      ccc[11:4]
            [5]      ccc[3:0], read_bl_len[3:0]
            --- union [6..11] ---
            v1: [6]  read_bl_partial[7], write_blk_misalign[6], read_blk_misalign[5], dsr_imp[4], reserved[3:2], c_size[11:10]
                [7]  c_size[9:2]
                [8]  c_size[1:0], vdd_r_curr_min[5:3], vdd_r_curr_max[2:0]
                [9]  vdd_w_curr_min[7:5], vdd_w_curr_max[4:2], c_size_mult[1:0] (hi)
                [10] c_size_mult[0] (lo), erase_blk_en[6], sector_size[5:0] (hi)
                [11] sector_size[0] (lo), wp_grp_size[6:0]
            v2: [6]  read_bl_partial[7], write_blk_misalign[6], read_blk_misalign[5], dsr_imp[4], reserved[3:0]
                [7]  reserved[5:0], c_size[21:16]
                [8]  c_size[15:8]
                [9]  c_size[7:0]
                [10] erase_blk_en[7], sector_size[6:0]
                [11] wp_grp_size[6:0]
            --- end union ---
            [12]     wp_grp_enable[7], reserved[6:5], r2w_factor[4:2], write_bl_len[1:0] (hi)
            [13]     write_bl_len[3:2] (lo), write_bl_partial[1], reserved[0]
            [14]     file_format_grp[7], copy[6], perm_write_protect[5], tmp_write_protect[4], file_format[3:2], reserved[1:0]
            [15]     crc[7:1], 1
 */
struct __attribute__((packed)) sd_csd {
    /* [0] */
    uint8_t reserved_0    : 6;
    uint8_t csd_structure : 2;

    /* [1..5] — общие для v1 и v2 */
    uint8_t taac;
    uint8_t nsac;
    uint8_t tran_speed;
    uint16_t read_bl_len : 4;   /* [5] bits 3:0 */
    uint16_t ccc         : 12;  /* [4] bits 7:0 + [5] bits 7:4 */

    /* [6..11] — различаются */
    union {
        struct __attribute__((packed)) {
            /* [6] */
            uint8_t c_size_hi      : 2;   /* c_size[11:10] */
            uint8_t reserved_6     : 2;
            uint8_t dsr_imp        : 1;
            uint8_t rd_blk_mis     : 1;
            uint8_t wr_blk_mis     : 1;
            uint8_t rd_bl_part     : 1;
            /* [7] */
            uint8_t c_size_mid;           /* c_size[9:2] */
            /* [8] */
            uint8_t vdd_r_max      : 3;
            uint8_t vdd_r_min      : 3;
            uint8_t c_size_lo      : 2;   /* c_size[1:0] */
            /* [9] */
            uint8_t c_size_mult_hi : 2;   /* c_size_mult[2:1] */
            uint8_t vdd_w_max      : 3;
            uint8_t vdd_w_min      : 3;
            /* [10] */
            uint8_t sector_size_hi : 6;
            uint8_t erase_blk_en   : 1;
            uint8_t c_size_mult_lo : 1;   /* c_size_mult[0] */
            /* [11] */
            uint8_t wp_grp_size    : 7;
            uint8_t sector_size_lo : 1;
        } v1;
        struct __attribute__((packed)) {
            /* [6] */
            uint8_t  reserved_6    : 4;
            uint8_t  dsr_imp       : 1;
            uint8_t  rd_blk_mis    : 1;
            uint8_t  wr_blk_mis    : 1;
            uint8_t  rd_bl_part    : 1;
            /* [7]: reserved[7:2], c_size[21:16] in bits [1:0] */
            uint8_t  c_size_hi     : 6;   /* c_size[21:16] — MSB 6 bits (spec says upper 6 bits are 0, lower 6 are c_size[21:16]) */
            uint8_t  reserved_7    : 2;
            /* [8]: c_size[15:8] */
            uint8_t  c_size_mid;
            /* [9]: c_size[7:0] */
            uint8_t  c_size_lo;
            /* [10] */
            uint8_t  sector_size   : 7;
            uint8_t  erase_blk_en  : 1;
            /* [11] */
            uint8_t  wp_grp_size   : 7;
            uint8_t  reserved_11   : 1;
        } v2;
    };

    /* [12..15] — общие для v1 и v2 */
    uint8_t  wr_bl_len_hi  : 2;   /* write_bl_len[3:2] */
    uint8_t  r2w_factor    : 3;
    uint8_t  reserved_12   : 2;
    uint8_t  wp_grp_enable : 1;

    uint8_t  reserved_13   : 5;
    uint8_t  wr_bl_partial : 1;
    uint8_t  wr_bl_len_lo  : 2;   /* write_bl_len[1:0] */

    uint8_t  reserved_14   : 2;
    uint8_t  file_format   : 2;
    uint8_t  tmp_wr_prot   : 1;
    uint8_t  perm_wr_prot  : 1;
    uint8_t  copy          : 1;
    uint8_t  file_fmt_grp  : 1;

    uint8_t  always1       : 1;
    uint8_t  crc7          : 7;
};

static_assert(sizeof(struct sd_csd) == 16, "struct sd_csd must be exactly 16 bytes");

static inline unsigned sd_csd_version(const struct sd_csd * c)
{
    return c->csd_structure;
}

static inline unsigned sd_csd_size_mb(const struct sd_csd * c)
{
    unsigned c_size = c->v2.c_size_lo;
    if (sd_csd_version(c) == 1) {
        /* CSD v2 (SDHC/SDXC): size = (c_size + 1) * 512 KB */
        c_size += (c->v2.c_size_hi << 16) + (c->v2.c_size_mid << 8);
        return (c_size + 1) / 2;
    } else {
        /* CSD v1 (SDSC) */
        c_size += (c->v2.c_size_hi << 10) + (c->v2.c_size_mid << 2);

        unsigned c_size_mult = (c->v1.c_size_mult_hi << 1) + c->v1.c_size_mult_lo;
        unsigned blocknr = (c_size + 1) * (1u << (c_size_mult + 2));
        return (blocknr * (1u << c->read_bl_len)) >> 20;
    }
}

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

static inline unsigned sd_detect(sd_cfg_t * cfg)
{
    if (gpio_get_state(cfg->detect)) {
        return 0;
    }
    return 1;
}
