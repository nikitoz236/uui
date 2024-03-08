#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#include "storage.h"
#include "storage_hw.h"

void print_hex_dump(const void * ptr, unsigned size)
{
    const uint8_t * p = ptr;
    unsigned col = 0;
    while (size--) {
        if (col == 0) {
            printf("%p: ", p);
        }
        printf("%02X", *p++);
        col++;
        if (col == 16) {
            col = 0;
            printf("\n");
        } else {
            printf(" ");
        }
    }
    if (col != 0) {
        printf("\n");
    }
}

/*
ЧТО НУЖНО ПРОВЕРИТЬ

    что файлы записываются в разные страницы
    что происходит стирание страниц

    для записи выбирается страница с наименьшим количеством свободного места

*/

char file_1[] = "ESP32-C3 SHA accelerator does not support interleaving message digest calculation at the level of \
individual message blocks when using DMA, which means you cannot insert new calculation before a \
complete DMA-SHA process (of one or more message blocks) completes. In this case, users who need \
interleaved operation are recommended to divide the message blocks and perform several DMA-SHA \
calculations, instead of trying to compute all the messages in one go. \
Single DMA-SHA calculation supports up to 63 data blocks. \
In contrast to the Typical SHA working mode, when the SHA accelerator is working under the DMA-SHA mode, \
all data read are completed via DMA. Therefore, users are required to configure the DMA controller following \
the description in Chapter 2 GDMA Controller (GDMA).";

char file_2[] = "some short file";

char file_3[] = "just another file";

char file_4[] = "file four";

struct file_for_write {
    file_id_t id;
    const void * data;
    unsigned len;
} files[] = {
    { 12, file_1, sizeof(file_1) },
    { 45, file_2, sizeof(file_2) },
    { 65099, file_3, sizeof(file_3) },
    { 567, file_4, sizeof(file_4) },
};

void file_write(unsigned n)
{
    if (n > 3) {
        return;
    }
    const struct file_for_write * f = &files[n];
    storage_write_file(f->id, f->data, f->len);
    storage_prepare_page();
}

int main ()
{
    printf("storage lib test\n");

    storage_init();

    for (int k = 0; k < 1000 ; k++) {
        unsigned rv = rand();
        unsigned n = (rv / 4) % 256;
        unsigned fidx = rv % 4;

        for (unsigned i = 0; i < n; i++) {
            // printf("\n!!! Writing cycle %d : %d file %d\n", k, i, files[fidx].id);
            file_write(fidx);
            storage_prepare_page();
        }

        // storage_init();
        // printf("\n\n\n\n\nINIT");
        // storage_print_info();
    }

    printf("\n\n\n\n\nREADY");
    storage_print_info();

    print_hex_dump(storage_page_to_pointer(0), 32);
    return 0;
}
