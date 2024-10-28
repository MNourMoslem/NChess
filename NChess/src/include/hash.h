#ifndef NCHESS_HASH_H
#define NCHESS_HASH_H

#include <stdlib.h>
#include <string.h>
#include "types.h"


typedef struct
{
    cuint64 key;
    int value;
    void* next;
} _NCH_Ht_item;

typedef struct
{
    _NCH_Ht_item** items;
    int size;
} _NCH_Ht;

int _NCH_Hashfunction(cuint64 key, int size)
{
    return (key * 5086ull) % size;
}

_NCH_Ht_item* _NCH_Ht_item_New(cuint64 key, int value){
    _NCH_Ht_item* item = (_NCH_Ht_item*)malloc(sizeof(_NCH_Ht_item));
    if (!item){
        return NULL;
    }

    item->key = key;
    item->value = value;
    item->next = NULL;

    return item;
}

_NCH_Ht* _NCH_Ht_New(int size){
    _NCH_Ht* table = (_NCH_Ht*)malloc(sizeof(_NCH_Ht));
    if(!table){
        return NULL;
    }

    table->items = (_NCH_Ht_item**)calloc(size, sizeof(_NCH_Ht_item*));
    if (!table->items){
        free(table);
        return NULL;
    }

    table->size = size;
}

void _NCH_Ht_Inesrt(_NCH_Ht* table ,cuint64 key, int value){
    int idx = _NCH_Hashfunction(key, table->size);

    _NCH_Ht_item* item = table->items[idx];
    if (item != NULL){
        if (item->key == key){
            item->value = value;
            return;
        }
        item = (_NCH_Ht_item*)item->next;
    }

    item = _NCH_Ht_item_New(key, value);
}

int _NCH_Ht_GetValue(_NCH_Ht* table ,cuint64 key){
    int idx = _NCH_Hashfunction(key, table->size);

    _NCH_Ht_item* item = table->items[idx];
    if (item != NULL){
        if (item->key == key){
            return item->value;
        }
        item = (_NCH_Ht_item*)item->next;
    }

    return -1;
}

_NCH_Ht_item* _NCH_Ht_GetItem(_NCH_Ht* table ,cuint64 key){
    int idx = _NCH_Hashfunction(key, table->size);

    _NCH_Ht_item* item = table->items[idx];
    if (item != NULL){
        if (item->key == key){
            return item;
        }
        item = (_NCH_Ht_item*)item->next;
    }

    return NULL;
}

int _NCH_Ht_AddValueToItem(_NCH_Ht* table, cuint64 key, int value){
    _NCH_Ht_item* item = _NCH_Ht_GetItem(table, key);
    if (item){
        item->value += value;
    }
    else{
        item = _NCH_Ht_item_New(key, value);
    }

    return item->value;
}

void _NCH_Ht_item_FreeAll(_NCH_Ht_item* item){
    if (item != NULL){
        _NCH_Ht_item_FreeAll((_NCH_Ht_item*)item->next);        
        free(item);
    }
}

void _NCH_Ht_Free(_NCH_Ht* table){
    if (table){
        for (int i = 0; i < table->size; i++){
            _NCH_Ht_item_FreeAll(table->items[i]);
        }
        free(table);
    }
}

#endif