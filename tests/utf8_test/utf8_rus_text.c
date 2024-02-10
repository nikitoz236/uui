#include <stdio.h>
#include <stdint.h>

int main() {
    char * rus_text = "Привет!";

    printf("text: %s \n", rus_text);
    // print hex dump of rus_text

    uint8_t * buf = (uint8_t*)rus_text;
    while (*buf != '\0') {
        printf("%02X ", *buf);
        buf++;
    }

    buf = (uint8_t*)rus_text;
    while (*buf != '\0') {
        printf("%02X ", *buf);
        printf("%c\n", *rus_text);
        buf++;
        rus_text++;
    }



    return 0;
}
