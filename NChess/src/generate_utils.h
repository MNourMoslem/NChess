#ifndef NCHESS_SRC_GENERATE_UTILS_H
#define NCHESS_SRC_GENERATE_UTILS_H

#include "core.h"
#include "config.h"
#include "types.h"
#include "board.h"

uint64
get_checkmap(Board* board, Side side, int king_idx, int all_occ);

uint64
get_allowed_pieces(Board* board);

uint64
get_allowed_squares(Board* board);

#endif