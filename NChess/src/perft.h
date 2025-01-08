#ifndef NCHESS_SRC_PERFT_H
#define NCHESS_SRC_PERFT_H

#include "board.h"

long long
Board_Perft(Board* board, int depth);

long long
Board_PerftPretty(Board* board, int depth);

long long
Board_PerftNoPrint(Board* board, int depth);

#endif