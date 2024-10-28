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
for (_NCH_MOVE move = {.map = piece_map, .idx = NCH_CTZLL(piece_map), .square = NCH_SQR(move.idx)};\
    move.idx < 64;\
    NCH_RMVFLG(move.map, move.square), move.idx = NCH_CTZLL(move.map), move.square = NCH_SQR(move.idx))

#define _NCH_MAP_LOOP_NAME_SPECEFIC(piece_map, move_name)\
for (_NCH_MOVE  move_name = {.map = piece_map, .idx = NCH_CTZLL(piece_map), .square = NCH_SQR(move_name.idx)};\
    move_name.idx < 64;\
    NCH_RMVFLG(move_name.map, move_name.square), move_name.idx = NCH_CTZLL(move_name.map), move_name.square = NCH_SQR(move_name.idx))

#endif