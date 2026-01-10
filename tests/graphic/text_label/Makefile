LIB = ../../../lib

# fonts.h
INC += $(LIB)/graphic/fonts
SRC += $(LIB)/graphic/fonts/*.c

# lcd_text_color.h
INC += $(LIB)/graphic/lcd_color
SRC += $(LIB)/graphic/lcd_color/*.c

# misc.h
INC += $(LIB)/common
SRC += $(LIB)/common/date_time.c
SRC += $(LIB)/common/val_text.c

INC += $(LIB)/graphic/text
SRC += $(LIB)/graphic/text/text_pointer.c
SRC += $(LIB)/graphic/text/font_config.c
SRC += $(LIB)/graphic/text/text_label.c

INC += $(LIB)/hw/include
SRC += $(LIB)/emu/hw/systick.c

TEST_NAME = test_text_label

include $(LIB)/emu/emu_dp_dep.mk
include $(LIB)/emu/graphic/emu_graphic.mk
