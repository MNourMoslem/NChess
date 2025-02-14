/*
    hash.c

    This file contains the implementation of the hash functions used in 
    the chess board dictionary for tracking board positions.
*/

#include "hash.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


// Computes a hash key for a given board position using bitwise operations.
// This function combines the bitboards for all pieces and sides into a single value.
NCH_STATIC_INLINE int
board_to_key(const uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB]){
    uint64 maps = 0ull;
    maps = ~bitboards[NCH_White][NCH_Pawn]
         ^ ~bitboards[NCH_White][NCH_Knight]
         ^ ~bitboards[NCH_White][NCH_Bishop]
         ^ ~bitboards[NCH_White][NCH_Rook]
         ^ ~bitboards[NCH_White][NCH_Queen]
         ^ ~bitboards[NCH_White][NCH_King]
         ^ ~bitboards[NCH_Black][NCH_Pawn]
         ^ ~bitboards[NCH_Black][NCH_Knight]
         ^ ~bitboards[NCH_Black][NCH_Bishop]
         ^ ~bitboards[NCH_Black][NCH_Rook]
         ^ ~bitboards[NCH_Black][NCH_Queen]
         ^ ~bitboards[NCH_Black][NCH_King];

    return (int)((((maps) >> 32) ^ maps) & 0x000000007FFFFFFF);
}


// Computes the index of a board position in the hash table.
NCH_STATIC_INLINE int
get_idx(const uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB]){
    return board_to_key(bitboards) % NCH_BOARD_DICT_SIZE;
}


// Checks whether the given bitboards match the stored board node.
NCH_STATIC_INLINE int
is_same_board(const uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB], const BoardNode* node){
    return (0 == memcmp(bitboards[NCH_White], node->bitboards[NCH_White], sizeof(node->bitboards[NCH_White])))
        && (0 == memcmp(bitboards[NCH_Black], node->bitboards[NCH_Black], sizeof(node->bitboards[NCH_Black])));
}

// Initializes the board dictionary by setting all nodes to empty.
void
BoardDict_Init(BoardDict* dict){
    for (int i = 0; i < NCH_BOARD_DICT_SIZE; i++){
        dict->nodes[i].empty = 1;
    }
}


// Frees all dynamically allocated nodes from the dictionary.
void
BoardDict_FreeExtra(BoardDict* dict){
    if (dict){
        BoardNode *node, *temp;
        for (int i = 0; i < NCH_BOARD_DICT_SIZE; i++){
            if (!dict->nodes[i].empty){
                node = dict->nodes[i].next;
                while (node)
                {
                    temp = node->next;
                    free(node);
                    node = temp;
                }
            }
        }
    }
}


// Retrieves the count of a given board position.
int
BoardDict_GetCount(const BoardDict* dict, const uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB]){
    int idx = get_idx(bitboards);
    BoardNode* node = dict->nodes + idx;
    if (node->empty)
        return -1;

    while (!is_same_board(bitboards, node))
    {   
        node = node->next;
        if (!node)
            return -1;
    }
    
    return node->count;
}


// Adds a board position to the dictionary.
int
BoardDict_Add(BoardDict* dict, const uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB]){
    int idx = get_idx(bitboards);
    BoardNode* node = dict->nodes + idx;
    if (!node->empty){
        while (!is_same_board(bitboards, node))
        {
            node = node->next;
            if (!node)
                break;
        }

        if (node){
            node->count++;
            return 0;
        }

        node = malloc(sizeof(BoardNode));
        if (!node)
            return -1;
    }

    memcpy(node->bitboards, bitboards, sizeof(node->bitboards));
    node->count = 1;
    node->empty = 0;
    node->next = NULL;

    return 0;
}


// Removes a board position from the dictionary.
int
BoardDict_Remove(BoardDict* dict, const uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB]){
    int idx = get_idx(bitboards);
    BoardNode* node = dict->nodes + idx;
    if (node->empty)
        return -1;

    BoardNode* prev = NULL;
    while (!is_same_board(bitboards, node))
    {
        if (!node->next)
            return -1;

        prev = node;
        node = node->next;
    }
    
    if (node->count > 1){
        node->count--;
    }
    else if (prev){
        prev->next = node->next;
        free(node);
    }
    else{
        node->empty = 1;
    }

    return 0;
}

void
BoardDict_Reset(BoardDict* dict){
    BoardNode *node, *temp;
    for (int i = 0; i < NCH_BOARD_DICT_SIZE; i++){
        if (!dict->nodes[i].empty){
            node = dict->nodes[i].next;
            while (node)
            {
                temp = node->next;
                free(node);
                node = temp;
            }
            dict->nodes[i].empty = 1;
        }
    }
}

int
BoardDict_CopyExtra(const BoardDict* src, BoardDict* dst){
    BoardNode* dn;
    int last;
    for (int i = 0; i < NCH_BOARD_DICT_SIZE; i++){
        const BoardNode* sn = src->nodes + i;
        if (!sn->empty){
            dn = dst->nodes + i;
            while (sn->next)
            {
                dn->next = (BoardNode*)malloc(sizeof(BoardNode));
                if (!dn->next){
                    last = i;
                    goto fail;
                }

                *dn->next = *sn->next;
                
                sn = sn->next;
                dn = dn->next;
            }
        }
    }

    return 0;

    fail:
        BoardNode* temp;
        for (int i = 0; i < last + 1; i++){
            dn = dst->nodes + i;
            if (!dn->empty && dn->next){
                dn = dn->next;
                while (dn)
                {
                    temp = dn;
                    dn = dn->next;
                    free(temp);
                }
            }
        }
        return -1;
}