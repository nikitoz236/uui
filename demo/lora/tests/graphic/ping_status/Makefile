LIB      = ../../../../../lib
PROTOCOL = ../../../protocol

INC += $(PROTOCOL)

# fonts
INC += $(LIB)/graphic/fonts
SRC += $(LIB)/graphic/fonts/*.c

# lcd_color
INC += $(LIB)/graphic/lcd_color
SRC += $(LIB)/graphic/lcd_color/*.c

# val_text
SRC += $(LIB)/common/val_text.c

# text pointer, font config, text label
INC += $(LIB)/graphic/text
SRC += $(LIB)/graphic/text/text_pointer.c
SRC += $(LIB)/graphic/text/font_config.c
SRC += $(LIB)/graphic/text/text_label.c

# timers, uptime
INC += $(LIB)/common
INC += $(LIB)/hw/include
SRC += $(LIB)/emu/hw/systick.c

TEST_NAME = test_ping_display

include $(LIB)/emu/emu_dp_dep.mk
include $(LIB)/emu/graphic/emu_graphic.mk
