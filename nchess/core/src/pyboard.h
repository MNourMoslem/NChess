#ifndef NCHESS_CORE_PYBOARD_H
#define NCHESS_CORE_PYBOARD_H

#define PY_SSIZE_CLEAN_H
#include <Python.h>

#include "nchess/board.h"

typedef struct
{
    PyObject_HEAD
    Board* board;
}PyBoard;

extern PyTypeObject PyBoardType;

PyObject*
PyBoard_FromBoardWithType(PyTypeObject *self, Board* board);

PyObject*
PyBoard_FromBoard(Board* board);

#endif