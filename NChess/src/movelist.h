#ifndef NCHESS_SRC_MOVELIST_H
#define NCHESS_SRC_MOVELIST_H

#include "types.h"
#include "config.h"
#include "core.h"
#include "move.h"

typedef struct MoveNode
{
    Move move;
    int gameflags;
    int fifty_count;
    uint8 castle;
    Square enp_sqr;
    Piece captured_piece;

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
MoveList_Append(MoveList* movelist, Move move, Square enp_sqr, Piece captured_piece,
                     int fifty_count, uint8 castle_flags, int flags);

void
MoveList_Pop(MoveList* movelist);

void
MoveList_Free(MoveList* movelist);

NCH_STATIC_INLINE MoveNode*
MoveList_Last(MoveList* movelist){
    if (movelist->len > 0){
        return movelist->last;
    }
    return 0;
}

#define MoveNode_FROM(node) Move_FROM((node)->move)
#define MoveNode_TO(node) Move_TO((node)->move)
#define MoveNode_CASTLE(node) Move_CASTLE((node)->move)
#define MoveNode_PRO_PIECE(node) Move_PRO_PIECE((node)->move)
#define MoveNode_ENP_SQR(node) ((node)->enp_sqr)
#define MoveNode_CAP_PIECE(node) ((node)->captured_piece)
#define MoveNode_FIFTY_COUNT(node) ((node)->fifty_count)
#define MoveNode_CASTLE_FLAGS(node) ((node)->castle)
#define MoveNode_GAME_FLAGS(node) ((node)->gameflags)

#endif