/*
    generate.h

    This file contains functions defeinitions for move generation.
    from generating pseudo moves to generating legal moves.
*/

#ifndef NCHESS_SRC_GENERATE_H
#define NCHESS_SRC_GENERATE_H

#include "core.h"
#include "config.h"
#include "types.h"
#include "board.h"
#include "loops.h"

// Generate all the legal moves for the current board.
int
Board_GenerateLegalMoves(Board* board, Move* moves);

// Generate all the pseudo moves for a piece on the board given its square.
Move*
Board_GeneratePseudoMovesOf(Board* board, Move* moves, Square sqr);

#endif