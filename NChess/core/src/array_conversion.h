#ifndef NCHESS_CORE_ARRAY_CONVERSION_H
#define NCHESS_CORE_ARRAY_CONVERSION_H

#include "nchess/nchess.h"
#define PY_SSIZE_CLEAN_T
#include <Python.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

PyObject*
create_numpy_array(void* data, npy_intp* dims, int ndim, enum NPY_TYPES dtype);

PyObject*
create_list_array(int* data, npy_intp* dims, int ndim);

int
parse_array_conversion_function_args(npy_intp nitems, npy_intp* dims, PyObject* args,
                                     PyObject* kwargs, int* reversed, int* as_list);

#endif // NCHESS_CORE_ARRAY_CONVERSION_H