#include "periph_header.h"
#include "periph_pclk.h"
#include "periph_rcc.h"
#include "periph_gpio.h"

#include "stm_usart.h"
#include "dp.h"
#include "sd_card.h"
#include "periph_spi.h"

const rcc_cfg_t rcc_cfg = {
    .hse_val = 8000000,
    .pll_src = PLL_SRC_PREDIV,
    .pll_prediv = 1,
    .pll_mul = 9,
    .sysclk_src = SYSCLK_SRC_PLL,
    .hclk_div = HCLK_DIV1,
    .apb_div = {
        APB_DIV2,
        APB_DIV1
    }
};

const gpio_t led = {
    .gpio = {
        .port = GPIO_PORT_C,
        .pin = 13
    },
    .cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .speed = GPIO_SPEED_HIGH,
        .type = GPIO_TYPE_PP
    }
};

const usart_cfg_t debug_usart = {
    .usart = USART1,
    .default_baud = 115200,
    .rx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 10
        },
        .cfg = {
            .mode = GPIO_MODE_INPUT,
            .pull = GPIO_PULL_NONE,
        }
    },
    .tx_pin = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_A,
            .pin = 9
        },
        .cfg = {
            .mode = GPIO_MODE_AF,
            .speed = GPIO_SPEED_HIGH,
            .type = GPIO_TYPE_PP,
        }
    },
    .tx_dma = {
        .dma_ch = 4,
    },
    .rx_dma = {
        .dma_ch = 5,
    },
    .pclk = PCLK_USART1,
    .irqn = USART1_IRQn
};

const sd_cfg_t sd = {
    .detect = &(const gpio_t) {
        .gpio = {
            .port = GPIO_PORT_B,
            .pin = 1
        },
        .cfg = {
            .mode = GPIO_MODE_INPUT,
            .pull = GPIO_PULL_UP
        }
    },
    .spi_dev = {
        .cs_pin = &(const gpio_t) {
            .gpio = {
                .port = GPIO_PORT_B,
                .pin = 12
            },
            .cfg = {
                .mode = GPIO_MODE_OUTPUT,
                .speed = GPIO_SPEED_HIGH,
                .type = GPIO_TYPE_PP
            }
        },
        .spi = &(const spi_cfg_t) {
            .spi = SPI2,
            .pclk = PCLK_SPI2,
            .clock_div = SPI_DIV_64,
            .pin_list = {
                [SPI_PIN_SCK] = &(const gpio_t){
                    .cfg = {
                        .mode = GPIO_MODE_AF,
                        .speed = GPIO_SPEED_HIGH,
                        .type = GPIO_TYPE_PP
                    },
                    .gpio = {
                        .port = GPIO_PORT_B,
                        .pin = 13
                    }
                },
                [SPI_PIN_MISO] = &(const gpio_t){
                    .cfg = {
                        .mode = GPIO_MODE_AF,
                        .pull = GPIO_PULL_NONE
                    },
                    .gpio = {
                        .port = GPIO_PORT_B,
                        .pin = 14
                    }
                },
                [SPI_PIN_MOSI] = &(const gpio_t){
                    .cfg = {
                        .mode = GPIO_MODE_AF,
                        .speed = GPIO_SPEED_HIGH,
                        .type = GPIO_TYPE_PP
                    },
                    .gpio = {
                        .port = GPIO_PORT_B,
                        .pin = 15
                    }
                }
            }
        }
    }
};

void __debug_usart_tx_data(const char * s, unsigned len)
{
    usart_tx(&debug_usart, s, len);
}

uint8_t buf[SD_SECTOR_SIZE];

void print_buf(unsigned sector)
{
    for (unsigned i = 0; i < (SD_SECTOR_SIZE / 16); i++) {
        dpx((i * 16) + (sector * SD_SECTOR_SIZE), 4);
        dp(" : ");
        dpxd(&buf[i * 16], 1, 16);
        dn();
    }
}

int main(void)
{
    rcc_apply_cfg(&rcc_cfg);

    pclk_ctrl(&(pclk_t)PCLK_AFIO, 1);

    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    pclk_ctrl(&(pclk_t)PCLK_IOPA, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPB, 1);
    pclk_ctrl(&(pclk_t)PCLK_IOPC, 1);

    pclk_ctrl(&(pclk_t)PCLK_DMA1, 1);
    __enable_irq();

    init_gpio(&led);
    gpio_set_state(&led, 1);

    usart_set_cfg(&debug_usart);

    dpn("SD card test project");

    if (sd_detect(&sd)) {
        dpn("SD card detected");
        init_sd(&sd);

        struct sd_cid cid;
        sd_read_cid(&sd, &cid);

        dp("CID: "); dpxd(&cid, 1, sizeof(cid)); dn();

        dp("    Manufacturer ID:                        "); dpx(cid.mid, 1);        dn();
        dp("    OEM/Application ID:                     "); dpx(cid.oid, 2);        dn();
        dp("    Product name:                           "); dpxd(cid.prn, 1, 5);    dn();
        dp("    Product revision:                       "); dpx(cid.prv, 1);        dn();
        dp("    Product serial number:                  "); dpx(cid.psn, 4);        dn();
        dp("    Manufacturing date (12 bit):            "); dpx(cid.mdt, 2);        dn();
        dp("    CRC7 checksum (7 bit, lsb - always 1 ): "); dpx(cid.crc, 1);        dn();

        struct sd_csd csd;
        sd_read_csd(&sd, &csd);
        dp("CSD: "); dpxd(&cid, 1, sizeof(csd)); dn();
        dp("    CSD structure:                          "); dpd(csd.csd_structure, 1); dn();

        sd_read_sector(&sd, 0, buf);
        print_buf(0);

        dp("size of sd_cid: "); dpd(sizeof(struct sd_cid), 4); dn();
        dp("size of sd_csd: "); dpd(sizeof(struct sd_csd), 4); dn();
        dp("size of sd_csd_v1: "); dpd(sizeof(struct sd_csd_v1), 4); dn();
        dp("size of sd_csd_v2: "); dpd(sizeof(struct sd_csd_v2), 4); dn();


        // if (buf[0x103] != 0x39) {
        //     buf[0x103] = 0x39;
        //     sd_write_sector(&sd, 0, buf);

        //     sd_read_sector(&sd, 1, buf);
        //     print_buf(1);

        //     sd_read_sector(&sd, 0, buf);
        //     print_buf(0);
        // }

    } else {
        dpn("SD card not detected");
    }

    while (1) {};

    return 0;
}
