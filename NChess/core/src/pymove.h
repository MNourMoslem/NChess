#ifndef NCHESS_CORE_PYMOVE_H
#define NCHESS_CORE_PYMOVE_H

#define PY_SSIZE_CLEAN_H
#include <Python.h>
#include "nchess/move.h"

typedef struct
{
    PyLongObject super;
}PyMove;

extern PyTypeObject PyMoveType;

PyMove*
PyMove_FromMove(Move move);

#define PyMove_Check(obj) PyObject_TypeCheck(obj, &PyMoveType)
#define PyMove_AsMove(obj) ((Move)PyLong_AsLongLong(obj))

PyObject*
PyMove_FromUCI(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject*
PyMove_FromArgs(PyObject* self, PyObject* args, PyObject* kwargs);

#endif // NCHESS_CORE_PYMOVE_H