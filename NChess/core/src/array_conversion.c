#include "array_conversion.h"
#include <numpy/arrayobject.h>

int
check_shape(PyObject* shape, int nitems, npy_intp* dims){
    if (!PySequence_Check(shape)){
        PyErr_Format(PyExc_TypeError,
            "shape expected to be a python sequence (list, tuple, ...). got %s",
            Py_TYPE(shape)->tp_name);

        return -1;
    }

    int ndim = PySequence_Length(shape);
    if (ndim > NPY_MAXDIMS){
        PyErr_Format(PyExc_ValueError,
            "could not create array from shape with ndim more then %i. got %i",
            NPY_MAXDIMS, ndim);
    
        return -1;
    }

    PyObject* item;
    int total = 1;
    for (int i = 0; i < ndim; i++){
        item = PySequence_GetItem(shape, i);
        if (!item){
            PyErr_SetString(PyExc_ValueError, "failed getitem from the inputted shape");
            return -1;
        }

        if (!PyNumber_Check(item)){
            PyErr_Format(PyExc_ValueError,
            "expected numbers as dimensions. got %s type",
            Py_TYPE(item)->tp_name);

            Py_DECREF(item);

            return -1;
        }

        dims[i] = PyLong_AsLong(PyNumber_Long(item));
        total *= dims[i];
        Py_DECREF(item);
    }

    if (total != nitems){
        PyErr_Format(PyExc_ValueError,
        "input shape expected to have %i number of items. got %i",
        nitems, total);

        return -1;
    }

    return ndim;
}

PyObject*
create_numpy_array(void* data, npy_intp* dims, int ndim, enum NPY_TYPES dtype){
    import_array();

    PyObject* numpy_array = PyArray_SimpleNewFromData(ndim, dims, dtype, data);
    if (!numpy_array) {
        return NULL;
    }

    PyArray_ENABLEFLAGS((PyArrayObject*)numpy_array, NPY_ARRAY_OWNDATA);
    return numpy_array;
}

NCH_STATIC PyObject*
create_list_array_recursive(int** data, npy_intp* dims, int dim, int roof){
    npy_intp size = dims[dim];
    PyObject* list = PyList_New(size);
    if (!list)
        return NULL;

    if (dim >= roof){
        for (npy_intp i = 0; i < size; i++){
            PyList_SetItem(list, i, PyLong_FromLong(*(*data)++));
        }
    }
    else{
        PyObject* item;
        for (npy_intp i = 0; i < size; i++){
            item = create_list_array_recursive(data, dims, dim+1, roof);
            if (!item){
                Py_DECREF(list);
                return NULL;
            }
            PyList_SetItem(list, i, item);
        }
    }

    return list;
}

PyObject*
create_list_array(int* data, npy_intp* dims, int ndim){
    return create_list_array_recursive(&data, dims, 0, ndim-1);
}

int
parse_board_conversion_function_args(int nitems, npy_intp* dims, PyObject* args,
                                     PyObject* kwargs, int* reversed, int* as_list)
{
    PyObject* shape = NULL;
    static char* kwlist[] = {"shape", "reversed", "as_list", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|Opp", kwlist, &shape, reversed, as_list)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the shape argument");
        }
        return NULL;
    }

    int ndim = 0;
    if (shape && !Py_IsNone(shape)){
        ndim = check_shape(shape, nitems, dims);
    }

    return ndim;
}

int
parse_bb_conversion_function_args(uint64* bb, int nitems, npy_intp* dims,
                                 PyObject* args, PyObject* kwargs, int* reversed, int* as_list)
{
    PyObject* shape = NULL;
    static char* kwlist[] = {"bb", "shape", "reversed", "as_list", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "K|Opp", kwlist, bb, &shape, reversed, as_list)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return -1;
    }

    int ndim = 0;
    if (shape && !Py_IsNone(shape)){
        ndim = check_shape(shape, nitems, dims);
    }

    return ndim;
}