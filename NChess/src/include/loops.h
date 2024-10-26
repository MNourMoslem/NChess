#ifndef NCHESS_LOOPS_H
#define NCHESS_LOOPS_H

#include "core.h"
#include "types.h"

typedef struct
{
    cuint64 map;
    int idx;
    cuint64 square;
} _NCH_MOVE;

#define _NCH_MAP_LOOP(piece_map)\
for (_NCH_MOVE move = {.map = piece_map, .idx = NCH_CTZLL(move.map), .square = NCH_SQR(move.idx)};\
    move.idx < 64;\
    NCH_RMVFLG(move.map, move.square), move.idx = NCH_CTZLL(move.map), move.square = NCH_SQR(move.idx))

#endif