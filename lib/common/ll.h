/*
    linked list

    односвязный список
*/

#pragma once
#include <stdint.h>

typedef struct ll_item ll_item_t;

struct ll_item {
    ll_item_t * next;
    uint8_t data[];
};

void ll_add(ll_item_t ** head, ll_item_t * item);
void ll_remove(ll_item_t ** head, ll_item_t * item);
void ll_process(ll_item_t ** head, unsigned(*process)(void *));
