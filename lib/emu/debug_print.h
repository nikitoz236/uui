#include <stdio.h>

#define debug_print(format, ...) \
    printf(format, ##__VA_ARGS__)

static inline void debug_print_hex(char * str, void * ptr, unsigned len)
{
    char * p = ptr;
    if (str) {
        debug_print("%s", str);
    }
    while (len--) {
        debug_print("%02X", *p);
        p++;
        if (len) {
            debug_print(" ");
        }
    }
    printf("\n");
}



typedef enum {
    CONSOLE_COLOR_BLACK,
    CONSOLE_COLOR_RED,
    CONSOLE_COLOR_GREEN,
    CONSOLE_COLOR_YELLOW,
    CONSOLE_COLOR_BLUE,
    CONSOLE_COLOR_MAGENTA,
    CONSOLE_COLOR_CYAN,
    CONSOLE_COLOR_WHITE,

    CONSOLE_COLOR_NONE
} console_color_t;


static inline void console_color_rst(void)
{
    debug_print("\x1b[m");
}

static inline void console_color(console_color_t fg, console_color_t bg)
{
    if (bg < CONSOLE_COLOR_NONE) {
        debug_print("\x1b[4%1dm", bg);
    }
    if (fg < CONSOLE_COLOR_NONE) {
        debug_print("\x1b[3%1dm", fg);
    }
}

#define debug_print_color(fg, bg, format, ...) \
    console_color(CONSOLE_COLOR_ ## fg, CONSOLE_COLOR_ ## bg); \
    debug_print(format, ##__VA_ARGS__); \
    console_color_rst();

