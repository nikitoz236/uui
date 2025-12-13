#include <stdio.h>
#include "array_size.h"

#include "ltree.h"

#define DP_NOTABLE
#include "dp.h"

uint8_t mem[1024];

char * test_result_text[] = { "FAIL", "OK" };

lt_desc_t desc = {
    .ctx_size = sizeof(unsigned),
    .desc_size = 0,
};

void print_lt_linear()
{
    dpn("lt linear dump:");
    lt_item_t * item = lt_item_from_offset(0);
    while (item) {
        dpd(lt_item_offset(item), 8);
        dp("  :  owner "); dpd(item->owner, 4);
        dp("  next "); dpd(item->next, 4);
        dp("  child "); dpd(item->child, 4);
        dp("  idx "); dpd(item->idx, 2);
        dp("  flags "); dpd(item->flags, 2);
        dp("  ctx: ");
        dpxd(&item->ctx, 4, item->desc->ctx_size / 4);
        dn();

        item = lt_next_in_mem(item);
    }
    dp("    lt size = "); dpd(lt_used(), 6); dn();
}

void print_tree(lt_item_t * item, unsigned level)
{
    while (item) {
        for (unsigned i = 0; i < level; i++) {
            dp("    ");
        }

        dp("+--"); dpd(lt_item_offset(item), 4);
        dp(" owner "); dpd(item->owner, 4);
        dp(" next "); dpd(item->next, 4);
        dp(" child "); dpd(item->child, 4);
        dp(" idx "); dpd(item->idx, 2);
        dp(" flags "); dpd(item->flags, 2);
        dp(" ctx: ");
        dpxd(&item->ctx, 4, item->desc->ctx_size / 4);
        dn();

        if (item->child) {
            print_tree(lt_child(item), level + 1);
        }
        item = lt_next(item);
    }
}

lt_item_t * create_lt(void)
{
    lt_init(mem, sizeof(mem));
    unsigned n = 0xAA00;
    lt_item_t * ci;

    lt_item_t * item_root_1 = lt_add(0, &desc);
    *(unsigned *)&item_root_1->ctx = n++;

    lt_item_t * item_child_1_1 = lt_add(item_root_1, &desc);
    *(unsigned *)&item_child_1_1->ctx = n++;

    lt_item_t * item_child_1_2 = lt_add(item_root_1, &desc);
    *(unsigned *)&item_child_1_2->ctx = n++;

    ci = lt_add(item_child_1_1, &desc);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(item_child_1_1, &desc);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(item_child_1_2, &desc);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(item_child_1_1, &desc);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(item_child_1_1, &desc);
    *(unsigned *)&ci->ctx = n++;

    lt_item_t * item_child_1_3 = lt_add(item_root_1, &desc);
    *(unsigned *)&item_child_1_3->ctx = n++;

    ci = lt_add(item_child_1_2, &desc);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(item_child_1_2, &desc);
    *(unsigned *)&ci->ctx = n++;

    return item_root_1;
}

void print_lt(void)
{
    print_lt_linear();

    dn();
    dpn("lt tree rep:");
    print_tree(lt_item_from_offset(0), 1);
}

unsigned check_linear_ctx(unsigned exp_ctx[], unsigned ctx_len)
{
    dn(); dpn("check tree state by linear ctx order");
    lt_item_t * item = lt_item_from_offset(0);
    unsigned idx = 0;
    unsigned total_res = 1;
    while (item) {
        unsigned * ctx = (unsigned *)item->ctx;
        unsigned res = 1;
        if (*ctx != exp_ctx[idx]) {
            res = 0;
            total_res = 0;
        }
        dp("  check ctx for idx "); dpd(idx, 3); dp(" = "); dpx(*ctx, 4); dp(" exp: "); dpx(exp_ctx[idx], 4); dp(" - "); dp(test_result_text[res]); dn();

        item = lt_next_in_mem(item);
        idx++;
    }
    unsigned res = 1;
    if (idx != ctx_len) {
        res = 0;
    }
    dp("  check lt len = "); dpd(idx, 3); dp(" exp: "); dpd(ctx_len, 3); dp(" - "); dp(test_result_text[res]); dn();
    return total_res;
}


/*

тесты

удаление всего
добавление нескольких корневых элементов - нельзя, иначе нельзя найти ссылающегося

удаление элемента в середине
удаление элемента в конце

добавление дочерних элементов


*/


unsigned exp_created[] = { 0xAA00, 0xAA01, 0xAA02, 0xAA03, 0xAA04, 0xAA05, 0xAA06, 0xAA07, 0xAA08, 0xAA09, 0xAA0A };


int main()
{
    printf("lt test\n");

    lt_item_t * root = create_lt();
    print_lt();

    check_linear_ctx(exp_created, ARRAY_SIZE(exp_created));

    // lt_item_t * n = lt_next(root);

    // dn();
    // dpn("node delete:");
    // print_tree(n, 1);




    // lt_delete_childs(item_root_1);

    // dn();
    // print_lt_linear();

    // dn();
    // dpn("lt tree rep:");
    // print_tree(item_root_1, 1);

    // lt_delete(n);

    // dn();
    // print_lt_linear();

    // dn();
    // dpn("lt tree rep:");
    // print_tree(root, 1);


    return 0;
}
