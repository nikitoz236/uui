
LIB = ../../../../lib


INC += .
SRC += lora_sx1262_tlora.c

PERIPH = $(LIB)/hw/esp32/periph
DRIVERS = $(LIB)/hw/drivers

INC += $(LIB)/common
INC += $(LIB)/hw/include
INC += $(DRIVERS)

SRC += $(LIB)/common/str_utils.c
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c

INC += $(PERIPH)
SRC += $(PERIPH)/dbg_usb_cdc_acm.c
SRC += $(PERIPH)/esp32_systime.c
SRC += $(PERIPH)/esp32_gpio.c
SRC += $(PERIPH)/esp32_pclk.c
SRC += $(PERIPH)/esp32_spi.c
SRC += $(PERIPH)/esp32_i2c.c

SRC += $(DRIVERS)/xl9555.c
SRC += $(DRIVERS)/sx1262.c
SRC += $(DRIVERS)/lora.c


include $(LIB)/hw/esp32/build_esp32.mk
