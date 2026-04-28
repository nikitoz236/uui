LIB = ../../../../../lib

TEST_NAME = test_print_char_flip_0_lsb_top

LCD_FB = 1
DEF += LCD_TOP_LSB

# fonts.h, font_*.c
INC += $(LIB)/graphic/fonts
SRC += $(LIB)/graphic/fonts/*.c

# print_char.h
INC += $(LIB)/graphic/text

# print_char.c (mono)
SRC += $(LIB)/graphic/lcd_mono/print_char.c

include $(LIB)/emu/emu_dp_dep.mk
include $(LIB)/emu/graphic/emu_graphic.mk
