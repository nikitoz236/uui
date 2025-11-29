
LIB = ../../../../lib


INC += .
SRC += test_s3_spi.c

PERIPH = $(LIB)/hw/esp32/periph

INC += $(LIB)/common
INC += $(LIB)/hw/include
INC += $(LIB)/hw/api

INC += $(PERIPH)
SRC += $(PERIPH)/esp32_spi.c
SRC += $(PERIPH)/esp32_pclk.c
SRC += $(PERIPH)/esp32_gpio.c

include $(LIB)/hw/esp32/build_esp32.mk
