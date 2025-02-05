#ifndef NCHESS_CORE_PYBOARD_H
#define NCHESS_CORE_PYBOARD_H

#define PY_SSIZE_CLEAN_H
#include <python.h>

#include "nchess/board.h"

typedef struct
{
    PyObject_HEAD
    Board* board;
}PyBoard;

extern PyTypeObject PyBoardType;

#endif