#ifndef NCHESS_SRC_GENERATE_H
#define NCHESS_SRC_GENERATE_H

#include "core.h"
#include "config.h"
#include "types.h"
#include "board.h"
#include "loops.h"

int
Board_GenerateLegalMoves(Board* board, Move* moves);

Move*
Board_GeneratePseudoMovesMapOf(Board* board, Move* moves, int idx);

#endif