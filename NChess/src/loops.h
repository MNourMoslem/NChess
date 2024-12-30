#ifndef NCHESS_SRC_LOOP_H
#define NCHESS_SRC_LOOP_H

#include "types.h"
#include "bit_operations.h"
#include "core.h"

#define LOOP_U64_T(map)\
int idx;\
for (uint64 map = map, idx = count_tbits(map); map != 0; NCH_RMVFLG(map, NCH_SQR(idx)), idx = count_tbits(map))

#endif