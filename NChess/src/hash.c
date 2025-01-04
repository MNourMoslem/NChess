#include "hash.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

NCH_STATIC_INLINE int
board_to_key(Board* board){
    uint64 maps = 0ull;
    maps = ~Board_WHITE_PAWNS(board)
         ^ ~Board_WHITE_KNIGHTS(board)
         ^ ~Board_WHITE_BISHOPS(board)
         ^ ~Board_WHITE_ROOKS(board)
         ^ ~Board_WHITE_QUEENS(board)
         ^ ~Board_WHITE_KING(board)
         ^ ~Board_BLACK_PAWNS(board)
         ^ ~Board_BLACK_KNIGHTS(board)
         ^ ~Board_BLACK_BISHOPS(board)
         ^ ~Board_BLACK_ROOKS(board)
         ^ ~Board_BLACK_QUEENS(board)
         ^ ~Board_BLACK_KING(board);

    return (int)((((maps) >> 32) ^ maps) & 0x000000007FFFFFFF);
}

NCH_STATIC_INLINE int
get_idx(Board* board){
    return board_to_key(board) % NCH_BOARD_DICT_SIZE;
}

NCH_STATIC_INLINE int
is_same_board(Board* board, BoardNode* node){
    return (0 == memcmp(board->bitboards[NCH_White], node->bitboards[NCH_White], sizeof(node->bitboards[NCH_White])))
        && (0 == memcmp(board->bitboards[NCH_Black], node->bitboards[NCH_Black], sizeof(node->bitboards[NCH_Black])));
}

NCH_STATIC_INLINE void
set_bitboards(Board* board, BoardNode* node){
    memcpy(node->bitboards[NCH_White], board->bitboards[NCH_White], sizeof(node->bitboards[NCH_White]));
    memcpy(node->bitboards[NCH_Black], board->bitboards[NCH_Black], sizeof(node->bitboards[NCH_Black]));
}

NCH_STATIC int
set_node(Board* board, BoardNode* node){
    if (node->empty || is_same_board(board, node)){
        set_bitboards(board, node);
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
            return set_node(board, node->next);
        }
        else{
            BoardNode* newnode = malloc(sizeof(BoardNode));
            if (!newnode){
                return -1;
            }

            set_bitboards(board, newnode);
            newnode->empty = 0;
            newnode->count = 1;
            newnode->next = NULL;
            node->next = newnode;
        }
    }
    return 0;
}

NCH_STATIC BoardNode*
get_node(Board* board, BoardNode* node){
    if (is_same_board(board, node) && !node->empty){
        return node;
    }
    else{
        if (node->next){
            return get_node(board, node->next);
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
BoardDict_GetCount(BoardDict* dict, Board* board){
    int idx = get_idx(board);
    BoardNode* node = get_node(board, &dict->nodes[idx]);
    if (!node){
        return -1;
    }
    return node->count;
}

int
BoardDict_Add(BoardDict* dict, Board* board){
    int idx = get_idx(board);
    return set_node(board, &dict->nodes[idx]);
}

int
BoardDict_Remove(BoardDict* dict, Board* board){
    int idx = get_idx(board);
    BoardNode* node = get_node(board, &dict->nodes[idx]);
    if (!node){
        return -1;
    }

    if (node->count > 1){
        printf("BoardDict_Remove count dicreased\n");
        node->count -= 1;
    }
    else{
        BoardNode* prev = &dict->nodes[idx];
        if (prev == node){
            if (!prev->next){
                prev->empty = 1;
                printf("BoardDict_Remove made it empty\n");
            }
            else{
                *prev = *(prev->next);
                printf("BoardDict_Remove prev\n");
                free(prev->next);
            }
        }
        else{
            while (prev->next != node)
            {   
                if (prev->next){
                    prev = prev->next;
                }
                else{
                    printf("BoardDict_Remove found nothing\n");
                    return -1;
                }
            }
            
            printf("BoardDict_Remove removed and relinked\n");
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
