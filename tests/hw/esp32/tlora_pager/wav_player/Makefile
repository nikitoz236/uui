
LIB = ../../../../../lib

INC += .
INC += ..
SRC += wav_player.c
SRC += ../tlora_hw.c

PERIPH = $(LIB)/hw/esp32/periph

INC += $(LIB)/common
INC += $(LIB)/hw/include

SRC += $(LIB)/common/str_utils.c
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c
SRC += $(LIB)/common/utf.c

INC += $(PERIPH)
SRC += $(PERIPH)/esp32_pclk.c
SRC += $(PERIPH)/esp32_gpio.c
SRC += $(PERIPH)/esp32_i2c.c
SRC += $(PERIPH)/esp32_spi.c
SRC += $(PERIPH)/esp32_i2s.c
SRC += $(PERIPH)/dbg_usb_cdc_acm.c
SRC += $(PERIPH)/esp32_systime.c

INC += $(LIB)/hw/drivers
SRC += $(LIB)/hw/drivers/sd_card.c
SRC += $(LIB)/hw/drivers/xl9555.c
SRC += $(LIB)/hw/drivers/es8311.c

INC += $(LIB)/fat32
SRC += $(LIB)/fat32/fat32.c

include $(LIB)/hw/esp32/build_esp32.mk
