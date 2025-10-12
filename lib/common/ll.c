#include "ll.h"

void ll_add(ll_item_t ** head, ll_item_t * item)
{
    ll_item_t * node = *head;
    while (node) {
        if (node == item) {
            // уже в списке
            return;
        }
        head = &node->next;
        node = node->next;
    }

    // добавляем последним элементом в список
    item->next = 0;
    *head = item;
}

void ll_remove(ll_item_t ** head, ll_item_t * item)
{
    ll_item_t * node = *head;
    while (node) {
        if (node == item) {
            *head = node->next;
            node->next = 0;
            return;
        }
        head = &node->next;
        node = node->next;
    }
    // не обнаружен в списке
}

void ll_process(ll_item_t ** head, unsigned(*process)(void *))
{
    ll_item_t * node = *head;
    while (node) {
        if (process(node->data)) {
            // нужно удалить из списка
            *head = node->next;
            node->next = 0;
            node = *head;
        } else {
            head = &node->next;
            node = node->next;
        }
    }
}
