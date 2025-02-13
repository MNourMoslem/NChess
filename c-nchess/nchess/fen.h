/*
    fen.h

    This file contains the function to create a board from a fen string.
    In the future a Board_AsFen would be added to this file to convert the board
    to a fen string.

    The way Board_FromFen behaves would change in the future to be able to create
    the board on the stack and without the need to allocate memory for the board.
*/

#ifndef NCHESS_SRC_FEN_H
#define NCHESS_SRC_FEN_H

#include "board.h"
#include "core.h"
#include "types.h"
#include "config.h"

// creates a board from a fen string
Board*
Board_FromFen(char* fen);

#endif