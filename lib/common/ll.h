/*
    linked list

    односвязный список без аллокации - ll_item_t встраивается
    как первое поле в структуру пользователя.

    head - указатель на переменную с указателем на первый элемент.

    ll_add    - добавить в конец, дедуплицирует (если уже в списке - no-op)
    ll_remove - убрать из списка
    ll_process(head, fn) - вызвать fn(node->data) для каждого,
                           если fn вернула 1 - удалить из списка
*/

#pragma once
#include <stdint.h>

typedef struct ll_item ll_item_t;

struct ll_item {
    ll_item_t * next;
    uint8_t data[];
};

// head - указатель на переменную с указателем на первый элемент списка

void ll_add(ll_item_t ** head, ll_item_t * item);
void ll_remove(ll_item_t ** head, ll_item_t * item);

// для каждого элемента списка вызывает функцию process
// если функция вернула ненулевое значение, элемент удаляется
void ll_process(ll_item_t ** head, unsigned(*process)(void *));
