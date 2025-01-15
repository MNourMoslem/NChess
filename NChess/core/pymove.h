#ifndef NCHESS_CORE_PYMOVE_H
#define NCHESS_CORE_PYMOVE_H

#define PY_SSIZE_CLEAN_H
#include <python.h>

#include "src/move.h"

typedef struct
{
    PyObject_HEAD
    Move move;
}PyMove;

extern PyTypeObject PyMoveType;

PyMove*
PyMove_New(Move move);

#define PyMove_Check(obj) PyObject_TypeCheck(obj, &PyMoveType)

#endif // NCHESS_CORE_PYMOVE_H