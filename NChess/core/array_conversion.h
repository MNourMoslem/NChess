#ifndef NCHESS_CORE_ARRAY_CONVERSION_H
#define NCHESS_CORE_ARRAY_CONVERSION_H

#include "src/nchess.h"
#define PY_SSIZE_CLEAN_T
#include <Python.h>
#include <numpy/arrayobject.h>

int
check_shape(PyObject* shape, int nitems, npy_intp* dims);

PyObject*
create_numpy_array(void* data, int* dims, int ndim, enum NPY_TYPES dtype);

PyObject*
create_list_array(int* data, int* dims, int ndim);

int
parse_array_conversion_function_args(int nitems, npy_intp* dims, PyObject* args,
                                     PyObject* kwargs, int* reversed, int* as_list);

#endif // NCHESS_CORE_ARRAY_CONVERSION_H