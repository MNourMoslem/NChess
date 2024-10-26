#ifndef NCHESS_HASH_H
#define NCHESS_HASH_H

#define CAPACITY 200 // Size of the HashTable.

int _NCH_Hashfunction(char* str)
{
    int i = 0;
    for (int j = 0; str[j]; j++)
        i += str[j];

    return i % CAPACITY;
}

typedef struct _NCH_Ht_item
{
    char* key;
    int value;
} _NCH_Ht_item;

typedef struct _NCH_HashTable
{
    _NCH_Ht_item** items;
    int size;
    int count;
} _NCH_HashTable;


#endif