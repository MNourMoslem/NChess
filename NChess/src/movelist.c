#include "movelist.h"
#include <stdlib.h>
#include "types.h"
#include <stdio.h>

uint32
gamestate_new(int is_enpassant, int is_promotion, Square enp_sqr, Piece captured_piece){
    uint32 state;
    state = is_enpassant | (is_promotion << 1) | (enp_sqr << 2) 
          | (captured_piece << 8);
    return state;
}

void*
MoveList_New(){
    MoveList* movelist = malloc(sizeof(MoveList));
    if (!movelist){
        return NULL;
    }

    movelist->first = NULL;
    movelist->last = NULL;
    movelist->len = 0;

    return movelist;
}

int MoveList_Append(MoveList* movelist, Move move, int is_enpassant,
                     int is_promotion, Square enp_sqr, Piece captured_piece,
                     int fifty_count, uint8 castle_flags, int flags){
    MoveNode* node = (MoveNode*)malloc(sizeof(MoveNode));
    if (!node) {
        return -1;
    }
    node->next = NULL;
    node->move = move;
    node->gamestate = gamestate_new(is_enpassant, is_promotion, enp_sqr, captured_piece);
    node->fifty_count = fifty_count;
    node->castle = castle_flags;
    node->gameflags = flags;

    if (movelist->len < 1) {
        node->prev = NULL;
        movelist->first = node;
        movelist->last = node;
    } else {
        movelist->last->next = node;
        node->prev = movelist->last;
        movelist->last = node;
    }

    movelist->len++;
    return 0;
}

void MoveList_Pop(MoveList* movelist) {
    if (movelist->len < 1) {
        return;
    }

    MoveNode* node = movelist->last;

    if (movelist->len == 1){
        movelist->first = NULL;
        movelist->last = NULL;
    }
    else{
        movelist->last = node->prev;
        movelist->last->next = NULL;
    }

    movelist->len--;
    free(node);
}


void
MoveList_Free(MoveList* movelist){
    if (movelist){
        MoveNode* node;
        while (movelist->len > 0)
        {
            node = movelist->last;
            movelist->last = node->prev;
            movelist->len--;
            free(node);
        }
        free(movelist);
    }
}