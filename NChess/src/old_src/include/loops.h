#ifndef NCHESS_SRC_INCLUDE_LOOPS_H
#define NCHESS_SRC_INCLUDE_LOOPS_H

#include "core.h"
#include "types.h"

typedef struct
{
    cuint64 map;
    int idx;
    cuint64 square;
} _NCH_MOVE;

typedef struct
{
    cuint64 map;
    cuint64 square;
} _NCH_MOVE_NO_IDX;

#define _NCH_MAP_LOOP(piece_map)\
for (_NCH_MOVE move = {.map = piece_map, .idx = NCH_CTZLL(piece_map), .square = NCH_SQR(move.idx)};\
    move.idx < 64;\
    NCH_RMVFLG(move.map, move.square), move.idx = NCH_CTZLL(move.map), move.square = NCH_SQR(move.idx))

#define _NCH_MAP_LOOP_NAME_SPECEFIC(piece_map, instance_name)\
for (_NCH_MOVE  instance_name = {.map = piece_map, .idx = NCH_CTZLL(piece_map), .square = NCH_SQR(instance_name.idx)};\
    instance_name.idx < 64;\
    NCH_RMVFLG(instance_name.map, instance_name.square), instance_name.idx = NCH_CTZLL(instance_name.map), instance_name.square = NCH_SQR(instance_name.idx))

#define _NCH_MAP_LOOP_NOIDX(piece_map)\
for (_NCH_MOVE_NO_IDX move = {.map = piece_map, .square = NCH_SQR(NCH_CTZLL(piece_map))};\
    move.map != 0ull;\
    NCH_RMVFLG(move.map, move.square), move.square = NCH_SQR(NCH_CTZLL(move.map)))

#endif