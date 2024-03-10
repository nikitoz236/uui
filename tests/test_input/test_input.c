#include <stdio.h>
#include "console_getc.h"

int main() {
    printf("Keyboard read one pressed key test\n");
    printf("        press [q] for exit\n");

    char key = 0;
    while (key != 'q') {
        key = console_getc();
        if (key) {
            printf("You pressed %c\n", key);
        }
    }

    return 0;
}
