#include <stdio.h>
#include "array_size.h"

#include "ltree.h"

#define DP_NOTABLE
#include "dp.h"

uint8_t mem[1024];

char * test_result_text[] = { "OK", "FAIL" };

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

void print_lt(void)
{
    print_lt_linear();

    dn();
    dpn("lt tree rep:");
    print_tree(lt_item_from_offset(0), 1);
}

unsigned check_linear_ctx(unsigned exp_ctx[], unsigned ctx_len)
{
    unsigned res = 0;
    unsigned total_res = 0;
    dn(); dpn("check tree state by linear ctx order");

    lt_item_t * item = lt_item_from_offset(0);
    unsigned idx = 0;
    while (item) {
        unsigned * ctx = (unsigned *)item->ctx;
        res = 0;
        if (*ctx != exp_ctx[idx]) {
            res = 1;
            total_res = 1;
        }
        dp("  check ctx for idx "); dpd(idx, 3); dp(" = "); dpx(*ctx, 4); dp(" exp: "); dpx(exp_ctx[idx], 4); dp(" - "); dp(test_result_text[res]); dn();

        item = lt_next_in_mem(item);
        idx++;
    }

    res = 0;
    if (idx != ctx_len) {
        total_res = 1;
        res = 1;
    }
    dp("  check lt len = "); dpd(idx, 3); dp(" exp: "); dpd(ctx_len, 3); dp(" - "); dp(test_result_text[res]); dn();

    res = 0;
    unsigned used = lt_used();
    if (used != (ctx_len * 16)) {
        total_res = 1;
        res = 1;
    }
    dp("  check lt used = "); dpd(used, 6); dp(" exp: "); dpd(ctx_len * 16, 6); dp(" - "); dp(test_result_text[res]); dn();

    return total_res;
}

lt_item_t * create_lt(void)
{
    lt_init(mem, sizeof(mem));
    unsigned n = 0xAA00;
    lt_item_t * ci;

    lt_item_t * root = lt_add(0, &desc);
    *(unsigned *)&root->ctx = n++;

    lt_item_t * child_1 = lt_add(root, &desc);
    *(unsigned *)&child_1->ctx = n++;

    lt_item_t * child_2 = lt_add(root, &desc);
    *(unsigned *)&child_2->ctx = n++;

    ci = lt_add(child_1, &desc);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(child_1, &desc);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(child_2, &desc);
    *(unsigned *)&ci->ctx = n++;

    lt_item_t * child_1_3 = lt_add(child_1, &desc);
    *(unsigned *)child_1_3->ctx = n++;

    ci = lt_add(child_1, &desc);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(root, &desc);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(child_2, &desc);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(child_1_3, &desc);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(child_1_3, &desc);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(child_2, &desc);
    *(unsigned *)&ci->ctx = n++;

    return root;
}


/*

тесты

удаление всего
добавление нескольких корневых элементов - нельзя, иначе нельзя найти ссылающегося

удаление элемента в середине
удаление элемента в конце

добавление дочерних элементов


*/


unsigned exp_created[] = { 0xAA00, 0xAA01, 0xAA02, 0xAA03, 0xAA04, 0xAA05, 0xAA06, 0xAA07, 0xAA08, 0xAA09, 0xAA0A, 0xAA0B, 0xAA0C };


unsigned exp_delete_1[] = { 0xAA00, 0xAA02, 0xAA05, 0xAA08, 0xAA09, 0xAA0C };

int main()
{
    printf("lt test\n");
    unsigned total_res = 0;

    lt_item_t * root = create_lt();
    print_lt();
    if (check_linear_ctx(exp_created, ARRAY_SIZE(exp_created))) {
        total_res = 1;
    }

    lt_item_t * n = lt_child_idx(root, 0);

    dn();
    dpn("node delete:");
    print_tree(n, 1);

    // lt_delete_childs(root);

    lt_delete(n);
    print_lt();

    if (check_linear_ctx(exp_delete_1, ARRAY_SIZE(exp_delete_1))) {
        total_res = 1;
    }

    // dn();
    // print_lt_linear();

    // dn();
    // dpn("lt tree rep:");
    // print_tree(root, 1);

    dn(); dp("TOTAL TEST RESULT - "); dp(test_result_text[total_res]); dn();
    return total_res;
}
