#ifndef NCHESS_SRC_LOOP_H
#define NCHESS_SRC_LOOP_H

#include "types.h"
#include "bit_operations.h"
#include "core.h"

#define LOOP_U64_T(map)\
for (uint64 map = map, sqr = get_ts1b(map); map != 0; NCH_RMVFLG(map, sqr), sqr = get_ls1b(map))

#endif