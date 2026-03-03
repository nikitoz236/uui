#include <stdio.h>
#include "array_size.h"

#include "ltree.h"

#define DP_NOTABLE
#include "dp.h"

uint8_t mem[1024];

char * test_result_text[] = { "OK", "FAIL" };

static const uint16_t test_ctx_size = sizeof(unsigned);
static const uint16_t * test_ctx_size_ptr = &test_ctx_size;

void print_lt_linear()
{
    lt_item_t * item = lt_item_from_offset(0);
    while (item) {
        dpd(lt_item_offset(item), 8);
        dp("  :  owner "); dpd(item->owner, 4);
        dp("  next "); dpd(item->next, 4);
        dp("  child "); dpd(item->child, 4);
        dp("  ctx: ");
        dpxd(&item->ctx, 4, **item->ctx_size / 4);
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
        dp(" ctx: ");
        dpxd(&item->ctx, 4, **item->ctx_size / 4);
        dn();

        if (item->child) {
            print_tree(lt_child(item), level + 1);
        }
        item = lt_next(item);
    }
}

void print_lt(void)
{
    dn();
    dpn("lt linear dump:");
    print_lt_linear();

    dn();
    dpn("lt tree rep:");
    print_tree(lt_item_from_offset(0), 1);
}

/*
    проверяет компактификацию буфера после мутаций
    проходит по памяти линейно (lt_next_in_mem), сравнивает ctx каждого элемента с ожидаемым массивом
    проверяет количество элементов и lt_used()
    не проверяет ссылочную структуру дерева (owner/child/next)
*/
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

/*
    рекурсивно обходит поддерево по ссылкам (lt_child / lt_next)
    для каждого элемента проверяет что owner указывает на родителя
    считает количество достижимых элементов в *count
*/
unsigned check_subtree(lt_item_t * item, unsigned expected_owner, unsigned * count)
{
    unsigned res = 0;

    while (item) {
        (*count)++;
        unsigned offset = lt_item_offset(item);
        unsigned r = (item->owner != expected_owner);
        if (r) res = 1;

        dp("  check owner for "); dpd(offset, 4);
        dp(" = "); dpd(item->owner, 4);
        dp(" exp: "); dpd(expected_owner, 4);
        dp(" - "); dp(test_result_text[r]); dn();

        lt_item_t * child = lt_child(item);
        if (child) {
            if (check_subtree(child, offset, count)) {
                res = 1;
            }
        }

        item = lt_next(item);
    }

    return res;
}

/*
    проверяет целостность ссылочной структуры дерева после мутаций
    обходит дерево рекурсивно по lt_child / lt_next начиная с корня
    проверяет:
    - owner каждого элемента указывает на его родителя
    - количество элементов достижимых по дереву совпадает с количеством в линейном буфере
      (нет потерянных нод, выпавших из дерева после компактификации)
*/
unsigned check_tree_integrity(void)
{
    unsigned res = 0;
    dn(); dpn("check tree integrity");

    unsigned tree_count = 0;
    if (check_subtree(lt_item_from_offset(0), 0, &tree_count)) {
        res = 1;
    }

    unsigned linear_count = 0;
    lt_item_t * item = lt_item_from_offset(0);
    while (item) {
        linear_count++;
        item = lt_next_in_mem(item);
    }

    unsigned r = (tree_count != linear_count);
    if (r) res = 1;
    dp("  tree count = "); dpd(tree_count, 3);
    dp(" linear count = "); dpd(linear_count, 3);
    dp(" - "); dp(test_result_text[r]); dn();

    return res;
}

lt_item_t * create_lt(void)
{
    lt_init(mem, sizeof(mem));
    unsigned n = 0xAA00;
    lt_item_t * ci;

    lt_item_t * root = lt_add(0, &test_ctx_size_ptr);
    *(unsigned *)&root->ctx = n++;

    lt_item_t * child_1 = lt_add(root, &test_ctx_size_ptr);
    *(unsigned *)&child_1->ctx = n++;

    lt_item_t * child_2 = lt_add(root, &test_ctx_size_ptr);
    *(unsigned *)&child_2->ctx = n++;

    ci = lt_add(child_1, &test_ctx_size_ptr);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(child_1, &test_ctx_size_ptr);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(child_2, &test_ctx_size_ptr);
    *(unsigned *)&ci->ctx = n++;

    lt_item_t * child_1_3 = lt_add(child_1, &test_ctx_size_ptr);
    *(unsigned *)child_1_3->ctx = n++;

    ci = lt_add(child_1, &test_ctx_size_ptr);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(root, &test_ctx_size_ptr);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(child_2, &test_ctx_size_ptr);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(child_1_3, &test_ctx_size_ptr);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(child_1_3, &test_ctx_size_ptr);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(child_2, &test_ctx_size_ptr);
    *(unsigned *)&ci->ctx = n++;

    return root;
}

void add_lt(lt_item_t * item, unsigned n)
{
    lt_item_t * ci;

    lt_item_t * child_1 = lt_add(item, &test_ctx_size_ptr);
    *(unsigned *)&child_1->ctx = n++;

    lt_item_t * child_2 = lt_add(item, &test_ctx_size_ptr);
    *(unsigned *)&child_2->ctx = n++;

    ci = lt_add(child_1, &test_ctx_size_ptr);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(child_1, &test_ctx_size_ptr);
    *(unsigned *)&ci->ctx = n++;

    ci = lt_add(child_2, &test_ctx_size_ptr);
    *(unsigned *)&ci->ctx = n++;
}

unsigned exp_created[] = { 0xAA00, 0xAA01, 0xAA02, 0xAA03, 0xAA04, 0xAA05, 0xAA06, 0xAA07, 0xAA08, 0xAA09, 0xAA0A, 0xAA0B, 0xAA0C };
unsigned exp_delete_1[] = { 0xAA00, 0xAA02, 0xAA05, 0xAA08, 0xAA09, 0xAA0C };
unsigned exp_add_1[] = { 0xAA00, 0xAA02, 0xAA05, 0xAA08, 0xAA09, 0xAA0C, 0xCC00, 0xCC01, 0xCC02, 0xCC03, 0xCC04, 0xDD00, 0xDD01, 0xDD02, 0xDD03, 0xDD04, 0xBB00, 0xBB01, 0xBB02, 0xBB03, 0xBB04 };
unsigned exp_delete_2[] = { 0xAA00, 0xAA02, 0xAA05, 0xAA08, 0xAA09, 0xAA0C, 0xCC00, 0xCC01, 0xCC04, 0xBB00, 0xBB01, 0xBB02, 0xBB03, 0xBB04 };
unsigned exp_delete_3[] = { 0xAA00, 0xAA08, 0xCC00, 0xCC01, 0xCC04 };

int main()
{
    printf("lt test\n");
    unsigned total_res = 0;

    lt_item_t * root = create_lt();
    print_lt();
    if (check_linear_ctx(exp_created, ARRAY_SIZE(exp_created))) {
        total_res = 1;
    }
    if (check_tree_integrity()) {
        total_res = 1;
    }

    lt_item_t * n = lt_child_idx(root, 0);

    dn();
    dpn("node delete:");
    print_tree(n, 1);

    lt_delete(n);
    print_lt();

    if (check_linear_ctx(exp_delete_1, ARRAY_SIZE(exp_delete_1))) {
        total_res = 1;
    }
    if (check_tree_integrity()) {
        total_res = 1;
    }

    add_lt(lt_child_idx(root, 1), 0xCC00);
    add_lt(
        lt_child_idx(
            lt_child_idx(
                lt_child_idx(
                    root,
                    1
                ),
                0
            ),
            1
        ),
        0xDD00
    );
    add_lt(lt_child_idx(lt_child_idx(root, 0), 0), 0xBB00);
    print_lt();

    if (check_linear_ctx(exp_add_1, ARRAY_SIZE(exp_add_1))) {
        total_res = 1;
    }
    if (check_tree_integrity()) {
        total_res = 1;
    }

    lt_delete_childs(
        lt_child_idx(
            lt_child_idx(
                root,
                1
            ),
            0
        )
    );
    print_lt();

    if (check_linear_ctx(exp_delete_2, ARRAY_SIZE(exp_delete_2))) {
        total_res = 1;
    }
    if (check_tree_integrity()) {
        total_res = 1;
    }

    lt_delete(lt_child_idx(root, 0));
    print_lt();
    if (check_linear_ctx(exp_delete_3, ARRAY_SIZE(exp_delete_3))) {
        total_res = 1;
    }
    if (check_tree_integrity()) {
        total_res = 1;
    }

    dn(); dp("TOTAL TEST RESULT - "); dp(test_result_text[total_res]); dn();
    return total_res;
}
