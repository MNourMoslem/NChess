/*
    fen.h

    This file contains the function to create a board from a fen string.
    In the future a Board_AsFen would be added to this file to convert the board
    to a fen string.

    The way Board_NewFen behaves would change in the future to be able to create
    the board on the stack and without the need to allocate memory for the board.
*/

#ifndef NCHESS_SRC_FEN_H
#define NCHESS_SRC_FEN_H

#include "board.h"
#include "core.h"
#include "types.h"
#include "config.h"

// creates a board from a FEN string. the function is dynamic and
// could deal with extra white spaces. FEN has to contain board
// pieces, side to play, castle rights. rest (en passant square,
// fifty count, nmoves) is optional.
// returns Board on success and NULL on failure
Board*
Board_NewFen(char* fen);

int
Board_FromFen(char* fen, Board* dst_board);

#endif