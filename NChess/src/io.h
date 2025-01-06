#ifndef NCHESS_SRC_IO_H
#define NCHESS_SRC_IO_H

#include "core.h"
#include "config.h"
#include "board.h"

void
Board_Print(Board* board);

void
Board_PrintMoves(Board* board);

void
Board_PrintInfo(Board *board);

int
Board_MovesAsString(Board* board, char buffer[][7]);

#endif