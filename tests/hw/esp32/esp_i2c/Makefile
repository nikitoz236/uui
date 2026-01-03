
LIB = ../../../../lib


INC += .
SRC += test_esp32_i2c.c

PERIPH = $(LIB)/hw/esp32/periph

INC += $(LIB)/common
INC += $(LIB)/hw/include

SRC += $(LIB)/common/str_utils.c
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c

INC += $(PERIPH)
SRC += $(PERIPH)/dbg_usb_cdc_acm.c
SRC += $(PERIPH)/esp32_systime.c

SRC += $(PERIPH)/esp32_gpio.c
SRC += $(PERIPH)/esp32_pclk.c
SRC += $(PERIPH)/esp32_i2c.c



include $(LIB)/hw/esp32/build_esp32.mk
