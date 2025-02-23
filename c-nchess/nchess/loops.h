/*
    loops.h

    This file contains the loop macros for the bitboards.
    These macros are shortcut for iterating over the set bits of a bitboard.
*/

#ifndef NCHESS_SRC_LOOP_H
#define NCHESS_SRC_LOOP_H

#include "types.h"
#include "bit_operations.h"
#include "core.h"

#define LOOP_U64_T(bitmap)\
idx = NCH_SQRIDX(bitmap);\
for (uint64 map = bitmap; map != 0; map &= map - 1, idx = NCH_SQRIDX(map))

#define LOOP_U64_NAMED(bb_name, idx_name, bb)\
idx_name = NCH_SQRIDX(bb);\
for (uint64 bb_name = bb; bb_name != 0; bb_name &= bb_name - 1, idx_name = NCH_SQRIDX(bb_name))

#endif