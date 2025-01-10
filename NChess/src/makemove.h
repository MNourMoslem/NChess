#ifndef NCHESS_SRC_MAKEMOVE_H
#define NCHESS_SRC_MAKEMOVE_H

#include "core.h"
#include "board.h"
#include "types.h"
#include "config.h"

void
_Board_MakeMove(Board* board, Move move);

void
Board_StepByMove(Board* board, Move move);

void
Board_Step(Board* board, char* move);

void
Board_Undo(Board* board);

int 
Board_IsMoveLegal(Board* board, Move move);

#endif