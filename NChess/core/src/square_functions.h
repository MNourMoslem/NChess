#ifndef NCHESS_CORE_SRC_SQUARE_FUNCTIONS_H
#define NCHESS_CORE_SRC_SQUARE_FUNCTIONS_H

#define PY_SSIZE_CLEAN_H
#include <Python.h>
#include "nchess/move.h"

PyObject* uci_as_square(PyObject* self, PyObject* args);
PyObject* square_file(PyObject* self, PyObject* args);
PyObject* square_rank(PyObject* self, PyObject* args);
PyObject* square_distance(PyObject* self, PyObject* args);
PyObject* square_mirror(PyObject* self, PyObject* args);

#endif // NCHESS_CORE_PYMOVE_H