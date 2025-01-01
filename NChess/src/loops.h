#ifndef NCHESS_SRC_LOOP_H
#define NCHESS_SRC_LOOP_H

#include "types.h"
#include "bit_operations.h"
#include "core.h"

#define LOOP_U64_T(bitmap)\
idx = NCH_SQRIDX(bitmap);\
for (uint64 map = bitmap; map != 0; NCH_RMVFLG(map, NCH_SQR(idx)), idx = NCH_SQRIDX(map))

#endif