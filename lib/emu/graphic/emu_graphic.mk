# должен быть определен путь к LIB и переменная TEST_NAME содержать имя файла теста

# api_lcd_color.h
INC += $(LIB)/hw/api

# forms.h
# lcd_text.h
INC += $(LIB)/graphic

# emu_lcd.h
# coord_type.h
INC += $(LIB)/emu/graphic/

AI_DIR = ai_frames

ifdef AI
SRC += $(LIB)/emu/graphic/emu_lcd_bitmap.c
DEF += AI_FRAMES_DIR=\"$(AI_DIR)\"
GFX_FLAGS = -lm
else
SRC += $(LIB)/emu/graphic/gfx.c
SRC += $(LIB)/emu/graphic/emu_lcd.c

GFX_FLAGS = -lX11 -lm -I /usr/X11R6/include
endif

CFLAGS += -Wno-pointer-to-int-cast

include $(LIB)/emu/emu.mk

%_app: %.c $(SRC)
	gcc $^ $(addprefix -I, $(INC)) $(addprefix -D, $(DEF)) $(GFX_FLAGS) $(CFLAGS) -o $@
