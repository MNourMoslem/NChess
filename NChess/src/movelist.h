#ifndef NCHESS_SRC_MOVELIST_H
#define NCHESS_SRC_MOVELIST_H

#include "types.h"
#include "config.h"

typedef struct MoveNode
{
    uint32 move;
    struct MoveNode* prev;
    struct MoveNode* next;
}MoveNode;

typedef struct
{
    MoveNode* first;
    MoveNode* last;
    int len;
}MoveList;


void*
MoveList_New();

int
MoveList_Append(MoveList* movelist, uint32 move);

void
MoveList_Pop(MoveList* movelist);

void
MoveList_Free(MoveList* movelist);

NCH_STATIC_INLINE uint32
MoveList_LastMove(MoveList* movelist){
    if (movelist->len > 0){
        return movelist->last->move;
    }
    return 0;
}

#endif