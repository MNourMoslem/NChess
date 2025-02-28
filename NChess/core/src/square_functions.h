#ifndef NCHESS_CORE_SRC_SQUARE_FUNCTIONS_H
#define NCHESS_CORE_SRC_SQUARE_FUNCTIONS_H

#define PY_SSIZE_CLEAN_H
#include <Python.h>
#include "nchess/move.h"

PyObject* square_from_uci(PyObject* self, PyObject* args);
PyObject* square_column(PyObject* self, PyObject* args);
PyObject* square_row(PyObject* self, PyObject* args);
PyObject* square_distance(PyObject* self, PyObject* args);
PyObject* square_mirror(PyObject* self, PyObject* args, PyObject* kwargs);

#endif // NCHESS_CORE_PYMOVE_H