#ifndef NCHESS_CORE_BB_FUNCTIONS_H
#define NCHESS_CORE_BB_FUNCTIONS_H

#include "nchess/types.h"

#define PY_SSIZE_CLEAN_T
#include <Python.h>

void
bb2array(uint64 bb, int* arr, int reverse);

uint64
bb_from_object(PyObject* obj);

PyObject* BB_FromArray(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* BB_FromSquares(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* BB_RookAttacks(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* BB_BishopAttacks(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* BB_QueenAttacks(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* BB_KingAttacks(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* BB_KnightAttacks(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* BB_PawnAttacks(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* BB_RookMask(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* BB_BishopMask(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* BB_RookRelevant(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* BB_BishopRelevant(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* BB_RookMagic(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* BB_BishopMagic(PyObject* self, PyObject* args, PyObject* kwargs);

#endif // NCHESS_CORE_BB_FUNCTIONS_H