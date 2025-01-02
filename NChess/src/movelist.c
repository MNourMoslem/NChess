#include "movelist.h"
#include <stdlib.h>
#include "types.h"
#include <stdio.h>

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

int MoveList_Append(MoveList* movelist, uint32 move) {
    MoveNode* node = (MoveNode*)malloc(sizeof(MoveNode));
    if (!node) {
        return -1;
    }
    node->next = NULL;
    node->move = move;

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