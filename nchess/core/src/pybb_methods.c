#include "bb_functions.h"
#include "pybb_methods.h"
#include "PyBB.h"
#include "array_conversion.h"
#include "common.h"

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

#define PY_SSIZE_CLEAN_T
#include <Python.h>

#define BB_SIZE NCH_SQUARE_NB

NCH_STATIC PyObject*
bb_as_array(PyObject* self, PyObject* args, PyObject* kwargs){
    npy_intp dims[NPY_MAXDIMS];
    int reversed, as_list;
    int ndim = parse_array_conversion_function_args(BB_SIZE, dims, args, kwargs, &reversed, &as_list);
    if (ndim < 0 || PyErr_Occurred()) {
        return NULL;
    }
    
    uint64 bb = BB_FromLong(self);
    
    if (!ndim){
        ndim = 1;
        dims[0] = BB_SIZE;
    }
    
    if (as_list){
        int data[BB_SIZE];
        bb2array(bb, data, reversed);
        return create_list_array(data, dims, ndim);
    }

    int* data = (int*)malloc(BB_SIZE * sizeof(int));
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

NCH_STATIC PyObject*
bb_more_than_one(PyObject* self, PyObject* args){
    uint64 bb = BB_FromLong(self);
    if (PyErr_Occurred()) {
        return NULL;
    }
    if (more_than_one(bb)) {Py_RETURN_TRUE;} Py_RETURN_FALSE;
}

NCH_STATIC PyObject*
bb_has_two_bits(PyObject* self, PyObject* args){
    uint64 bb = BB_FromLong(self);
    if (PyErr_Occurred()) {
        return NULL;
    }
    if (has_two_bits(bb)) {Py_RETURN_TRUE;} Py_RETURN_FALSE;
}

NCH_STATIC PyObject*
bb_get_last_bit(PyObject* self, PyObject* args) {
    uint64 bb = BB_FromLong(self);
    if (PyErr_Occurred()) {
        return NULL;
    }
    uint64 last_bit = get_last_bit(bb);
    int idx = NCH_SQRIDX(last_bit);
    return PyLong_FromLong(idx);
}

NCH_STATIC PyObject*
bb_count_bits(PyObject* self, PyObject* args){
    uint64 bb = BB_FromLong(self);
    if (PyErr_Occurred()) {
        return NULL;
    }

    return PyLong_FromLong(count_bits(bb));
}

NCH_STATIC PyObject*
bb_is_filled(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* sqr;
    static char* kwlist[] = {"square", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &sqr)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }
    
    Square s = pyobject_as_square(sqr);
    CHECK_NO_SQUARE_ERR(s, NULL)
    
    uint64 bb = BB_FromLong(self);
    if (PyErr_Occurred()) {
        return NULL;
    }

    if (bb & NCH_SQR(s)) {Py_RETURN_TRUE;} Py_RETURN_FALSE;
}

NCH_STATIC PyObject* 
bb_to_squares(PyObject* self, PyObject* args, PyObject* kwargs){
    int as_set = 0;
    NCH_STATIC char* kwlist[] = {"as_set", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|p", kwlist, &as_set)){
        return NULL;
    }

    uint64 bb = BB_FromLong(self);
    if (PyErr_Occurred()) {
        return NULL;
    }

    int idx;
    Py_ssize_t i = 0;
    PyObject* item;
    if (as_set){
        PyObject* set = PySet_New(NULL);
        if (!set){
            if (!PyErr_Occurred()){
                PyErr_NoMemory();
            }
            return NULL;
        }

        LOOP_U64_T(bb){
            item = PyLong_FromLong(idx);
            PySet_Add(set, item);
            Py_DECREF(item);
        }

        return set;
    }
    else{
        PyObject* list = PyList_New(count_bits(bb));
        if (!list){
            if (!PyErr_Occurred()){
                PyErr_NoMemory();
            }
            return NULL;
        }

        LOOP_U64_T(bb){
            item = PyLong_FromLong(idx);
            PyList_SetItem(list, i++, item);
        }

        return list;
    }


}

NCH_STATIC PyObject*
bb_set_square(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* sqr;
    static char* kwlist[] = {"square", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &sqr)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }
    
    Square s = pyobject_as_square(sqr);
    if (s == NCH_NO_SQR){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to convert input to square");
        }
        return NULL;
    }
    
    uint64 bb = BB_FromLong(self);
    if (PyErr_Occurred()) {
        return NULL;
    }
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(bb | NCH_SQR(s));
}

NCH_STATIC PyObject*
bb_remove_square(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* sqr;
    static char* kwlist[] = {"square", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &sqr)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }
    
    Square s = pyobject_as_square(sqr);
    if (s == NCH_NO_SQR){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to convert input to square");
        }
        return NULL;
    }
    
    uint64 bb = BB_FromLong(self);
    if (PyErr_Occurred()) {
        return NULL;
    }
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(bb & ~NCH_SQR(s));
}

PyMethodDef pybb_methods[] = {
    {"more_than_one", (PyCFunction)bb_more_than_one, METH_NOARGS                 , NULL},
    {"has_two_bits" , (PyCFunction)bb_has_two_bits , METH_NOARGS                 , NULL},
    {"get_last_bit" , (PyCFunction)bb_get_last_bit , METH_NOARGS                 , NULL},
    {"count_bits"   , (PyCFunction)bb_count_bits   , METH_NOARGS                 , NULL},
    {"as_array"     , (PyCFunction)bb_as_array     , METH_VARARGS | METH_KEYWORDS, NULL},
    {"is_filled"    , (PyCFunction)bb_is_filled    , METH_VARARGS | METH_KEYWORDS, NULL},
    {"to_squares"   , (PyCFunction)bb_to_squares   , METH_VARARGS | METH_KEYWORDS, NULL},
    {"set_square"   , (PyCFunction)bb_set_square   , METH_VARARGS | METH_KEYWORDS, NULL},
    {"remove_square", (PyCFunction)bb_remove_square, METH_VARARGS | METH_KEYWORDS, NULL},
    {NULL           , NULL                         , 0                           , NULL},
};
