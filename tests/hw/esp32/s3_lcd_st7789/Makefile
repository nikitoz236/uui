
LIB = ../../../../lib


INC += .
SRC += test_s3_lcd_st7789.c

PERIPH = $(LIB)/hw/esp32/periph

INC += $(LIB)/common
INC += $(LIB)/hw/include

SRC += $(LIB)/common/str_utils.c
SRC += $(LIB)/common/str_val.c
SRC += $(LIB)/common/str_val_buf.c

SRC += $(LIB)/hw/esp32/systime.c

INC += $(PERIPH)
SRC += $(PERIPH)/esp32_spi.c
SRC += $(PERIPH)/esp32_pwm.c
SRC += $(PERIPH)/esp32_pclk.c
SRC += $(PERIPH)/esp32_gpio.c

INC += $(LIB)/hw/api
INC += $(LIB)/hw/drivers
SRC += $(LIB)/hw/drivers/lcd_spi.c
SRC += $(LIB)/hw/drivers/lcd_ST7789.c

INC += $(LIB)/graphic/fonts
SRC += $(LIB)/graphic/fonts/fonts.c
SRC += $(LIB)/graphic/fonts/font_5x7.c

INC += $(LIB)/graphic
SRC += $(LIB)/graphic/lcd_text.c

INC += $(LIB)/graphic/lcd_color
SRC += $(LIB)/graphic/lcd_color/lcd_text_color.c

include $(LIB)/hw/esp32/build_esp32.mk
