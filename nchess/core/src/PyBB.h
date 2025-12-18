#ifndef NCHESS_CORE_SRC_BB_OBJECT_H
#define NCHESS_CORE_SRC_BB_OBJECT_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

typedef struct {
    PyLongObject super;  // This makes it inherit from `int`
} PyBitBoard;

extern PyTypeObject PyBitBoardType;

PyBitBoard* PyBitBoard_FromUnsignedLongLong(unsigned long long value);

#define BB_FromLong(obj) ((unsigned long long)PyLong_AsUnsignedLongLong(obj))

#endif
