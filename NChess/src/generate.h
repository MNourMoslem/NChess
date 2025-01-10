#ifndef NCHESS_SRC_GENERATE_H
#define NCHESS_SRC_GENERATE_H

#include "core.h"
#include "config.h"
#include "types.h"
#include "board.h"
#include "loops.h"

void
generate_moves(Board* board);

int
Board_GenerateLegalMoves(Board* board, Move* moves);

int
Board_CountLegalMoves(Board* board);

#endif