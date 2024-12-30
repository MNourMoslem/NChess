#ifndef NCHESS_SRC_GENERATE_H
#define NCHESS_SRC_GENERATE_H

#include "core.h"
#include "config.h"
#include "types.h"
#include "board.h"
#include "loops.h"

int
generate_pseudo_moves(Board* board, uint64 allowd_pieces);

#endif