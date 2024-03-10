#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

struct termio original_termio;

static void change_termio_cfg(void)
{
    int fd = fileno(stdin);
    ioctl(fd, TCGETA, &original_termio);
    struct termio zap;
    zap = original_termio;
    zap.c_cc[VMIN] = 0;
    zap.c_cc[VTIME] = 0;
    zap.c_lflag = 0;
    ioctl(fd, TCSETA, &zap);
}

static void restore_termio_cfg(void)
{
    int fd = fileno(stdin);
    ioctl(fd, TCSETA, &original_termio);
}

char console_getc(void)
{
    char key = 0;
    change_termio_cfg();
    int size = read(STDIN_FILENO, &key, 1);
    if (size == 0) {
        key = 0;
    }
    restore_termio_cfg();
    return key;
}
