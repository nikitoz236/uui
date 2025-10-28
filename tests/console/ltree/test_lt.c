#include <stdio.h>

#include "ltree.h"

#define DP_NOTABLE
#include "dp.h"

uint8_t mem[1024];


lt_desc_t desc = {
    .ctx_size = sizeof(unsigned),
    .desc_size = 0,
};

unsigned item_size_in_mem(lt_item_t * item);
lt_item_t * item_from_offset(unsigned offset);
unsigned item_offset(lt_item_t * item);

void print_lt_linear()
{
    dpn("lt linear dump:");
    unsigned offset = 0;
    while (offset < lt_used()) {
        lt_item_t * item = item_from_offset(offset);
        dpd(offset, 8);
        dp("  :  owner "); dpd(item->owner, 4);
        dp("  next "); dpd(item->next, 4);
        dp("  child "); dpd(item->child, 4);
        dp("  idx "); dpd(item->idx, 2);
        dp("  flags "); dpd(item->flags, 2);
        dp("  ctx: ");
        dpxd(&item->ctx, 4, item->desc->ctx_size / 4);
        dn();

        offset += item_size_in_mem(item);
    }
}

void print_tree(lt_item_t * item, unsigned level)
{
    while (item) {
        for (unsigned i = 0; i < level; i++) {
            dp("    ");
        }

        dp("+--"); dpd(item_offset(item), 4);
        dp(" owner "); dpd(item->owner, 4);
        dp(" next "); dpd(item->next, 4);
        dp(" child "); dpd(item->child, 4);
        dp(" idx "); dpd(item->idx, 2);
        dp(" flags "); dpd(item->flags, 2);
        dp(" ctx: ");
        dpxd(&item->ctx, 4, item->desc->ctx_size / 4);
        dn();

        if (item->child) {
            print_tree(item_from_offset(item->child), level + 1);
        }
        item = lt_next(item);
    }
}

int main()
{
    printf("lt test\n");
    lt_init(mem, sizeof(mem));

    unsigned n = 0xAA00;

    lt_item_t * item_root_1 = lt_add(0, &desc);
    *(unsigned *)&item_root_1->ctx = n++;

    lt_item_t * item_root_2 = lt_add(0, &desc);
    *(unsigned *)&item_root_2->ctx = n++;

    lt_item_t * item_child_1_1 = lt_add(item_root_1, &desc);
    *(unsigned *)&item_child_1_1->ctx = n++;

    lt_item_t * item_child_1_2 = lt_add(item_root_1, &desc);
    *(unsigned *)&item_child_1_2->ctx = n++;

    lt_item_t * item_child_2_1 = lt_add(item_root_2, &desc);
    *(unsigned *)&item_child_2_1->ctx = n++;

    lt_item_t * item_child_2_2 = lt_add(item_root_2, &desc);
    *(unsigned *)&item_child_2_2->ctx = n++;

    lt_item_t * item_child_1_3 = lt_add(item_root_1, &desc);
    *(unsigned *)&item_child_1_3->ctx = n++;

    print_lt_linear();


    print_tree(item_root_1, 1);

    return 0;
}
