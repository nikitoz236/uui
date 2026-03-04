
LIB = ../../../lib
PROTOCOL = ../protocol
TLORA_HW = ../../../tests/hw/esp32/tlora_pager

INC += .
INC += $(TLORA_HW)
INC += $(PROTOCOL)
SRC += tlora_lcd_ping.c
SRC += $(TLORA_HW)/tlora_hw.c
SRC += $(PROTOCOL)/lora_ping_pong.c

PERIPH = $(LIB)/hw/esp32/periph
DRIVERS = $(LIB)/hw/drivers

INC += $(LIB)/common
INC += $(LIB)/hw/include
INC += $(DRIVERS)

SRC += $(LIB)/common/str_utils.c
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c

INC += $(PERIPH)
SRC += $(PERIPH)/esp32_spi.c
SRC += $(PERIPH)/esp32_pwm.c
SRC += $(PERIPH)/esp32_pclk.c
SRC += $(PERIPH)/esp32_gpio.c
SRC += $(PERIPH)/esp32_i2c.c
SRC += $(PERIPH)/dbg_usb_cdc_acm.c
SRC += $(PERIPH)/esp32_systime.c

SRC += $(DRIVERS)/tca8418_kbd.c
SRC += $(DRIVERS)/aw9364.c
SRC += $(DRIVERS)/xl9555.c
SRC += $(DRIVERS)/bq25896.c
SRC += $(DRIVERS)/sx1262.c
SRC += $(DRIVERS)/lora.c

INC += $(LIB)/hw/api
SRC += $(DRIVERS)/lcd_spi.c
SRC += $(DRIVERS)/lcd_st7796.c

INC += $(LIB)/graphic/fonts
SRC += $(LIB)/graphic/fonts/fonts.c
SRC += $(LIB)/graphic/fonts/font_5x7.c

INC += $(LIB)/graphic
SRC += $(LIB)/graphic/lcd_text.c

INC += $(LIB)/graphic/lcd_color
SRC += $(LIB)/graphic/lcd_color/lcd_text_color.c

INC += $(LIB)/graphic/text
SRC += $(LIB)/graphic/text/text_pointer.c
SRC += $(LIB)/graphic/text/font_config.c

include $(LIB)/hw/esp32/build_esp32.mk
