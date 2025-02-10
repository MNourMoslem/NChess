#ifndef NCHESS_SRC_FEN_H
#define NCHESS_SRC_FEN_H

#include "board.h"
#include "core.h"
#include "types.h"
#include "config.h"

Board*
Board_FromFen(char* fen);

#endif