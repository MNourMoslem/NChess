#include "bb_functions.h"
#include "src/bit_operations.h"
#include "array_conversion.h"
#include "common.h"

NCH_STATIC int
parse_bb(uint64* bb, PyObject* args){
    if (!PyArg_ParseTuple(args, "K", bb)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return -1;
    }
    return 0;
}

PyObject* 
BB_AsArray(PyObject* self, PyObject* args, PyObject* kwargs){
    int reversed = 0;
    int as_list = 0;
    uint64 bb;

    int nitems = NCH_SQUARE_NB;
    npy_intp dims[NPY_MAXDIMS];
    int ndim = parse_bb_conversion_function_args(&bb, nitems, dims, args, kwargs, &reversed, &as_list);

    if (ndim < 0)
        return NULL;

    if (!ndim){
        ndim = 1;
        dims[0] = nitems;
    }

    if (as_list){
        int data[NCH_SQUARE_NB];
        bb2array(bb, data, reversed);
        return create_list_array(data, dims, ndim);
    }
    else{
        int* data = (int*)malloc(nitems * sizeof(int));
        if (!data){
            PyErr_NoMemory();
            return NULL;
        }

        bb2array(bb, data, reversed);
        
        PyObject* array = create_numpy_array(data, dims, ndim, NPY_INT);
        if (!array){
            free(data);
            if (!PyErr_Occurred()){
                PyErr_SetString(PyExc_RuntimeError, "Failed to create array");
            }
            return NULL;
        }

        return array;
    }
}

PyObject*
BB_MoreThenOne(PyObject* self, PyObject* args){
    uint64 bb;
    if (parse_bb(&bb, args) < 0)
        return NULL;

    return PyBool_FromLong(more_then_one(bb));
}

PyObject*
BB_HasTwoBits(PyObject* self, PyObject* args){
    uint64 bb;
    if (parse_bb(&bb, args) < 0)
        return NULL;

    return PyBool_FromLong(has_two_bits(bb));
}

PyObject*
BB_GetTSB(PyObject* self, PyObject* args){
    uint64 bb;
    if (parse_bb(&bb, args) < 0)
        return NULL;

    return PyLong_FromLong(get_ts1b(bb));
}

PyObject*
BB_GetLSB(PyObject* self, PyObject* args){
    uint64 bb;
    if (parse_bb(&bb, args) < 0)
        return NULL;

    return PyLong_FromLong(get_ls1b(bb));
}

PyObject*
BB_CountBits(PyObject* self, PyObject* args){
    uint64 bb;
    if (parse_bb(&bb, args) < 0)
        return NULL;

    return PyLong_FromLong(count_bits(bb));
}


PyObject*
BB_IsFilled(PyObject* self, PyObject* args, PyObject* kwargs){
    uint64 bb;
    PyObject* sqr;
    static const* kwlist[] = {"bb", "square", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "KO", kwlist, &bb, &sqr)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    Square s = pyobject_as_square(sqr);
    if (s == NCH_NO_SQR)
        Py_RETURN_NONE;

    return PyBool_FromLong(bb & NCH_SQR(s));
}