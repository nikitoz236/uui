LIB = ../../../../lib

TEST_NAME = test_text_print

LCD_FB = 1
DEF += LCD_TOP_LSB

# fonts.h, font_*.c
INC += $(LIB)/graphic/fonts
SRC += $(LIB)/graphic/fonts/*.c

# forms.h, forms_split.h
INC += $(LIB)/graphic/forms
SRC += $(LIB)/graphic/forms/*.c

# print_char.h, text_print.h, font_config.h, text_pointer.h
INC += $(LIB)/graphic/text
SRC += $(LIB)/graphic/text/text_pointer.c
SRC += $(LIB)/graphic/text/font_config.c
SRC += $(LIB)/graphic/text/text_print.c

# print_char.c (mono)
SRC += $(LIB)/graphic/lcd_mono/print_char.c

include $(LIB)/emu/emu_dp_dep.mk
include $(LIB)/emu/graphic/emu_graphic.mk
