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
Board_Step(Board* board, char* move_str);


// Undoes the last move played. If there is no move, it does nothing.
void
Board_Undo(Board* board);


// Checks whether a move is legal to be played.  
// The move does not require MoveType information.  
// If the move is valid, it resets the MoveType of the given Move object.  
//  
// For example if the move is a castle type and the type is not specified,  
// this function sets its type before returning the result.  
//  
// Returns 1 if the move is legal and 0 otherwise.  
int  
Board_CheckAndMakeMoveLegal(Board* board, Move* move_ptr);  


// Declares all legal moves of a square to a moves array.
// Returns the number of moves.
int
Board_GetMovesOf(Board* board, Square s, Move* moves);

#endif