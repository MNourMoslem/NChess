/*
    io.h

    This file containes functions for io purposes such as printing as string conversions.
*/

#ifndef NCHESS_SRC_IO_H
#define NCHESS_SRC_IO_H

#include "core.h"
#include "config.h"
#include "board.h"

void
Board_Print(Board* board);

void
Board_AsString(Board* board, char* buffer);

#endif