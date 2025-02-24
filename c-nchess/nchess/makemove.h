/*
    makemove.h

    This file contains declarations of all functions related to a Board Move,
    such as making or undoing moves, finding moves, etc.
*/

#ifndef NCHESS_SRC_MAKEMOVE_H
#define NCHESS_SRC_MAKEMOVE_H

#include "core.h"
#include "board.h"
#include "types.h"
#include "config.h"

// Makes a move regardless of whether it is legal or not,
// as long as the move squares (from, to) are valid.
// Otherwise, it will result in undefined behavior.
void
_Board_MakeMove(Board* board, Move move);

// Makes a move only if it is legal; otherwise, the move won't be played.
// Returns 1 if the move has been played and 0 if not.
int
Board_StepByMove(Board* board, Move move);

// Makes a move from UCI only if the move is legal; otherwise, the move won't be played.
// Returns 1 if the move has been played and 0 if not.
int
Board_Step(Board* board, char* move);

// Undoes the last move played. If there is no move, it does nothing.
void
Board_Undo(Board* board);

// Checks if a move is legal to be played or not. Move does not require
// MoveType information.
// Returns a new move that contains MoveType information if the input move
// is legal and 0 if not.
Move 
Board_IsMoveLegal(Board* board, Move move);

// Declares all legal moves of a square to a moves array.
// Returns the number of moves.
int
Board_GetMovesOf(Board* board, Square s, Move* moves);

#endif