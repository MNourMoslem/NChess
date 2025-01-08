#ifndef NCHESS_SRC_MAKEMOVE_H
#define NCHESS_SRC_MAKEMOVE_H

#include "core.h"
#include "board.h"
#include "types.h"
#include "config.h"

void
Board_StepByMove(Board* board, Move move);

void
Board_Step(Board* board, char* move);

void
Board_Undo(Board* board);

#endif