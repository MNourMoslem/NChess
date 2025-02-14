#ifndef NCHESS_SRC_MOVELIST_H
#define NCHESS_SRC_MOVELIST_H

#include "types.h"
#include "config.h"
#include "core.h"
#include "move.h"
#include <stdlib.h>

#define NCH_MOVELIST_SIZE 400

typedef struct MoveNode
{
    Move move;
    PositionInfo pos_info;

    struct MoveNode* prev;
    struct MoveNode* next;
}MoveNode;

typedef struct
{
    MoveNode nodes[NCH_MOVELIST_SIZE];
    int len;

    MoveNode* extra;
    MoveNode* last_extra;
}MoveList;

void
MoveList_Init(MoveList* movelist);

int
MoveList_Append(MoveList* movelist, Move move, PositionInfo pos_info);

void
MoveList_Pop(MoveList* movelist);

MoveNode*
MoveList_Get(MoveList* movelist, int idx);

void
MoveList_Free(MoveList* movelist);

void
MoveList_Reset(MoveList* movelist);

int
MoveList_CopyExtra(const MoveList* src, MoveList* dst);

NCH_STATIC_INLINE MoveNode*
MoveList_Last(MoveList* movelist){
    if (movelist->len <= 0)
        return NULL;

    if (movelist->len <= NCH_MOVELIST_SIZE)
        return movelist->nodes + movelist->len - 1;

    return movelist->last_extra;
}

#define MoveNode_FROM(node) Move_FROM((node)->move)
#define MoveNode_TO(node) Move_TO((node)->move)
#define MoveNode_CASTLE(node) Move_CASTLE((node)->move)
#define MoveNode_PRO_PIECE(node) Move_PRO_PIECE((node)->move)
#define MoveNode_ENP_SQR(node) ((node)->pos_info->enp_sqr)
#define MoveNode_CAP_PIECE(node) ((node)->pos_info->captured_piece)
#define MoveNode_FIFTY_COUNT(node) ((node)->pos_info->fifty_count)
#define MoveNode_CASTLE_FLAGS(node) ((node)->pos_info->castle)
#define MoveNode_GAME_FLAGS(node) ((node)->pos_info->gameflags)

#endif