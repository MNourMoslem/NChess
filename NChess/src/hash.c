#include "hash.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

NCH_STATIC_INLINE int
board_to_key(uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB]){
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

NCH_STATIC_INLINE int
get_idx(uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB]){
    return board_to_key(bitboards) % NCH_BOARD_DICT_SIZE;
}

NCH_STATIC_INLINE int
is_same_board(uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB], BoardNode* node){
    return (0 == memcmp(bitboards[NCH_White], node->bitboards[NCH_White], sizeof(node->bitboards[NCH_White])))
        && (0 == memcmp(bitboards[NCH_Black], node->bitboards[NCH_Black], sizeof(node->bitboards[NCH_Black])));
}

NCH_STATIC_INLINE void
set_bitboards(uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB], BoardNode* node){
    memcpy(node->bitboards[NCH_White], bitboards[NCH_White], sizeof(node->bitboards[NCH_White]));
    memcpy(node->bitboards[NCH_Black], bitboards[NCH_Black], sizeof(node->bitboards[NCH_Black]));
}

NCH_STATIC int
set_node(uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB], BoardNode* node){
    if (node->empty || is_same_board(bitboards, node)){
        set_bitboards(bitboards, node);
        if (node->empty){
            node->count = 1;
            node->empty = 0;
            node->next = NULL;
        }
        else{
            node->count += 1;
        }
    }
    else{
        if (node->next){
            return set_node(bitboards, node->next);
        }
        else{
            BoardNode* newnode = malloc(sizeof(BoardNode));
            if (!newnode){
                return -1;
            }

            set_bitboards(bitboards, newnode);
            newnode->empty = 0;
            newnode->count = 1;
            newnode->next = NULL;
            node->next = newnode;
        }
    }
    return 0;
}

NCH_STATIC BoardNode*
get_node(uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB], BoardNode* node){
    if (node->empty){
        return NULL;
    }
    
    if (is_same_board(bitboards, node)){
        return node;
    }
    else{
        if (node->next){
            return get_node(bitboards, node->next);
        }
        return NULL;
    }
}


BoardDict*
BoardDict_New(){
    BoardDict* dict = malloc(sizeof(BoardDict));
    if (!dict){
        return NULL;
    }

    for (int i = 0; i < NCH_BOARD_DICT_SIZE; i++){
        dict->nodes[i].empty = 1;
    }

    return dict;
}

void
BoardDict_Free(BoardDict* dict){
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
        free(dict);
    }
}

int
BoardDict_GetCount(BoardDict* dict, uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB]){
    int idx = get_idx(bitboards);
    BoardNode* node = get_node(bitboards, &dict->nodes[idx]);
    if (!node){
        return -1;
    }
    return node->count;
}

int
BoardDict_Add(BoardDict* dict, uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB]){
    int idx = get_idx(bitboards);
    return set_node(bitboards, &dict->nodes[idx]);
}

int
BoardDict_Remove(BoardDict* dict, uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB]){
    int idx = get_idx(bitboards);
    BoardNode* node = get_node(bitboards, &dict->nodes[idx]);
    if (!node){
        return -1;
    }

    if (node->count > 1){
        node->count -= 1;
    }
    else{
        BoardNode* prev = &dict->nodes[idx];
        if (prev == node){
            if (!prev->next){
                prev->empty = 1;
            }
            else{
                BoardNode* temp = prev->next;
                *prev = *temp;
                free(temp);
            }
        }
        else{
            while (prev->next != node)
            {   
                if (prev->next){
                    prev = prev->next;
                }
                else{
                    return -1;
                }
            }
            prev->next = prev->next->next;
            free(node);
        }
    }
    return 0;
};

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
