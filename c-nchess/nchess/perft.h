/*
    perft.h

    This file provides functions for running Perft (performance test) calculations 
    on a given chess position. Perft is used to validate move generation by 
    counting the number of legal moves at a given depth.
*/

#ifndef NCHESS_SRC_PERFT_H
#define NCHESS_SRC_PERFT_H

#include "board.h"


// Computes the number of legal moves up to a given depth and prints the result.
// Returns the total number of legal moves up to the given depth.
long long
Board_Perft(Board* board, int depth);


// Computes the number of legal moves up to a given depth and prints the result 
// in a formatted style, where commas are added for every three digits.
// Returns the total number of legal moves up to the given depth.
long long
Board_PerftPretty(Board* board, int depth);


// Computes the number of legal moves up to a given depth without printing anything.
// Returns the total number of legal moves up to the given depth.
long long
Board_PerftNoPrint(Board* board, int depth);


// Computes the number of legal moves up to a given depth and formats the result as a string.
// buffer: pointer to the output buffer
// buffer_size: size of the buffer
// pretty: if non-zero, formats numbers with commas
// Returns the total number of legal moves up to the given depth.
long long
Board_PerftAsString(Board* board, int depth, char* buffer, size_t buffer_size, int pretty);

#endif // NCHESS_SRC_PERFT_H
