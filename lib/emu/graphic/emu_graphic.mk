# должен быть определен путь к LIB и переменная TEST_NAME содержать имя файла теста
# LCD_FB=1 - моно через page-oriented framebuffer (lcd_fb)
# AI=1     - AI bitmap режим вместо X11

# api_lcd_color.h
INC += $(LIB)/hw/api

# forms.h, xy_type.h
INC += $(LIB)/graphic

# emu_lcd.h
# gfx.h
# color_type.h
# coord_type.h (LCD_FB)
INC += $(LIB)/emu/graphic/

SRC += $(LIB)/emu/graphic/emu_graphic.c

ifdef LCD_FB
# flex_wrap.h (для LCD_FB_CREATE)
INC += $(LIB)/common
# lcd_fb.h
INC += $(LIB)/graphic/lcd_mono
SRC += $(LIB)/emu/graphic/emu_fb_lcd.c
SRC += $(LIB)/graphic/lcd_mono/lcd_fb.c
else
SRC += $(LIB)/emu/graphic/emu_lcd_color.c
endif

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
