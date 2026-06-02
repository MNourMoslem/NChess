/*
    hash.c

    This file contains the implementation of the hash functions used in 
    the chess board dictionary for tracking board positions.
*/

#include "hash.h"
#include <stdlib.h>
#include <string.h>
#include "memory.h"

// Computes a hash key for a given board position using bitwise operations.
// This function combines the bitboards for all pieces and sides into a single value.
NCH_STATIC_INLINE int
board_to_key(const uint64 bitboards[NCH_PIECE_NB]){
    uint64 maps = ~bitboards[NCH_WPawn]
                ^ ~bitboards[NCH_WKnight]
                ^ ~bitboards[NCH_WBishop]
                ^ ~bitboards[NCH_WRook]
                ^ ~bitboards[NCH_WQueen]
                ^ ~bitboards[NCH_WKing]
                ^ ~bitboards[NCH_BPawn]
                ^ ~bitboards[NCH_BKnight]
                ^ ~bitboards[NCH_BBishop]
                ^ ~bitboards[NCH_BRook]
                ^ ~bitboards[NCH_BQueen]
                ^ ~bitboards[NCH_BKing];

    return (int)(maps % NCH_BOARD_DICT_SIZE);
}

// Checks whether the given bitboards match the stored board node.
NCH_STATIC_INLINE int
is_same_board(const uint64 bitboards[NCH_PIECE_NB], const BoardNode* node){
    return !memcmp(bitboards, node->bitboards, sizeof(node->bitboards));
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
                    NCH_FREE(node);
                    node = temp;
                }
                dict->nodes[i].next = NULL;
            }
        }
    }
}


// Retrieves the count of a given board position.
int
BoardDict_GetCount(const BoardDict* dict, const uint64 bitboards[NCH_PIECE_NB]){
    int idx = board_to_key(bitboards);
    const BoardNode* node = dict->nodes + idx;
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
BoardDict_Add(BoardDict* dict, const uint64 bitboards[NCH_PIECE_NB]){
    int idx = board_to_key(bitboards);
    BoardNode* head = dict->nodes + idx;

    // If bucket is empty, initialize in-place without allocation
    if (head->empty) {
        memcpy(head->bitboards, bitboards, sizeof(head->bitboards));
        head->count = 1;
        head->empty = 0;
        head->next = NULL;
        return 0;
    }

    // Traverse chain to find existing node or reach tail
    BoardNode* prev = NULL;
    BoardNode* node = head;
    while (!is_same_board(bitboards, node)) {
        prev = node;
        if (!node->next) {
            node = NULL; // will append new node after prev
            break;
        }
        node = node->next;
    }

    // If found, increment count
    if (node) {
        node->count++;
        return 0;
    }

    // Not found: append new node to the chain
    BoardNode* new_node = (BoardNode*)NCH_MALLOC(sizeof(BoardNode));
    if (!new_node)
        return -1;

    memcpy(new_node->bitboards, bitboards, sizeof(new_node->bitboards));
    new_node->count = 1;
    new_node->empty = 0;
    new_node->next = NULL;
    prev->next = new_node;
    return 0;
}


// Removes a board position from the dictionary.
int
BoardDict_Remove(BoardDict* dict, const uint64 bitboards[NCH_PIECE_NB]){
    int idx = board_to_key(bitboards);
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
        NCH_FREE(node);
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
                NCH_FREE(node);
                node = temp;
            }
            dict->nodes[i].next = NULL;
            dict->nodes[i].count = 0;
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
                dn->next = (BoardNode*)NCH_MALLOC(sizeof(BoardNode));
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
        ; // Empty statement required for C99 compliance
        BoardNode* temp;
        for (int i = 0; i < last + 1; i++){
            dn = dst->nodes + i;
            if (!dn->empty && dn->next){
                dn = dn->next;
                while (dn)
                {
                    temp = dn;
                    dn = dn->next;
                    NCH_FREE(temp);
                }
            }
        }
        return -1;
}