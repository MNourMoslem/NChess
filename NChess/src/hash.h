#ifndef NCHESS_SRC_HASH_H
#define NCHESS_SRC_HASH_H

#include "types.h"
#include "core.h"

#define NCH_BOARD_DICT_SIZE 100

typedef struct BoardNode
{
    int empty;
    uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB];
    int count;

    struct BoardNode* next;
}BoardNode;

typedef struct
{
    BoardNode nodes[NCH_BOARD_DICT_SIZE];
}BoardDict;

BoardDict*
BoardDict_New();

void
BoardDict_Free(BoardDict* dict);

int
BoardDict_GetCount(BoardDict* dict, uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB]);

int
BoardDict_Add(BoardDict* dict, uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB]);

int
BoardDict_Remove(BoardDict* dict, uint64 bitboards[NCH_SIDES_NB][NCH_PIECE_NB]);

void
BoardDict_Reset(BoardDict* dict);

#endif