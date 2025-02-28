#include "bb_functions.h"
#include "pybb.h"
#include "array_conversion.h"
#include "common.h"

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

#define PY_SSIZE_CLEAN_T
#include <Python.h>

PyBitBoard* PyBitBoard_FromUnsignedLongLong(unsigned long long value){
    PyObject* obj = PyLong_FromUnsignedLongLong(value);
    if (!obj){
        PyErr_SetString(
            PyExc_ValueError,
            "Falied to create a bitboard object"
        );
        return NULL;
    }
    obj->ob_type = &PyBitBoardType;
    return obj;
}

NCH_STATIC PyObject*
bb_as_array(PyObject* self, PyObject* args, PyObject* kwargs){
    npy_intp nitems = NCH_SQUARE_NB;
    npy_intp dims[NPY_MAXDIMS];
    int reversed, as_list;
    int ndim = parse_array_conversion_function_args(nitems, dims, args, kwargs, &reversed, &as_list);
    uint64 bb = PyLong_AsUnsignedLongLong(self);
    if (PyErr_Occurred()) {
        return NULL;
    }

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

NCH_STATIC PyObject*
bb_more_then_one(PyObject* self, PyObject* args){
    uint64 bb = PyLong_AsUnsignedLongLong(self);
    if (PyErr_Occurred()) {
        return NULL;
    }
    return PyBool_FromLong(more_then_one(bb));
}

NCH_STATIC PyObject*
bb_has_two_bits(PyObject* self, PyObject* args){
    uint64 bb = PyLong_AsUnsignedLongLong(self);
    if (PyErr_Occurred()) {
        return NULL;
    }
    return PyBool_FromLong(has_two_bits(bb));
}

NCH_STATIC PyObject*
bb_get_last_bit(PyObject* self, PyObject* args) {
    uint64_t bb = PyLong_AsUnsignedLongLong(self);
    if (PyErr_Occurred()) {
        return NULL;
    }
    uint64_t last_bit = get_last_bit(bb);
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(last_bit);
}

NCH_STATIC PyObject*
bb_count_bits(PyObject* self, PyObject* args){
    uint64 bb = PyLong_AsUnsignedLongLong(self);
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
    if (s == NCH_NO_SQR)
        Py_RETURN_NONE;
    
    uint64 bb = PyLong_AsUnsignedLongLong(self);
    if (PyErr_Occurred()) {
        return NULL;
    }
    return PyBool_FromLong(bb & NCH_SQR(s));
}

NCH_STATIC PyObject* 
bb_to_squares(PyObject* self, PyObject* args){
    uint64 bb = PyLong_AsUnsignedLongLong(self);
    if (PyErr_Occurred()) {
        return NULL;
    }

    PyObject* list = PyList_New(count_bits(bb));
    if (!list)
        return NULL;

    int idx;
    Py_ssize_t i = 0;
    LOOP_U64_T(bb){
        PyList_SetItem(list, i++, PyLong_FromLong(idx));
    }

    return list;
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
    
    uint64 bb = PyLong_AsUnsignedLongLong(self);
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
    
    uint64 bb = PyLong_AsUnsignedLongLong(self);
    if (PyErr_Occurred()) {
        return NULL;
    }
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(bb & ~NCH_SQR(s));
}

PyMethodDef bitboard_methods[] = {
    {"as_array"     , (PyCFunction)bb_as_array     , METH_VARARGS | METH_KEYWORDS, NULL},
    {"more_then_one", (PyCFunction)bb_more_then_one, METH_NOARGS                 , NULL},
    {"has_two_bits" , (PyCFunction)bb_has_two_bits , METH_NOARGS                 , NULL},
    {"get_last_bit" , (PyCFunction)bb_get_last_bit , METH_NOARGS                 , NULL},
    {"count_bits"   , (PyCFunction)bb_count_bits   , METH_NOARGS                 , NULL},
    {"is_filled"    , (PyCFunction)bb_is_filled    , METH_VARARGS | METH_KEYWORDS, NULL},
    {"to_squares"   , (PyCFunction)bb_to_squares   , METH_NOARGS                 , NULL},
    {"set_square"   , (PyCFunction)bb_set_square   , METH_VARARGS | METH_KEYWORDS, NULL},
    {"remove_square", (PyCFunction)bb_remove_square, METH_VARARGS | METH_KEYWORDS, NULL},
    {NULL           , NULL                         , 0                           , NULL},
};

NCH_STATIC PyObject*
bb_iter(PyObject* self){
    // just create a tuple and iterate over it
    uint64 bb = PyLong_AsUnsignedLongLong(self);
    if (PyErr_Occurred()) {
        return NULL;
    }
    PyObject* tuple = PyTuple_New(count_bits(bb));
    if (!tuple){
        return NULL;
    }

    Py_ssize_t i = 0;
    Square idx;
    LOOP_U64_T(bb){
        PyTuple_SetItem(tuple, i++, square_to_pyobject(idx));
    }

    // iterate the tuple
    return PyObject_GetIter(tuple);
}

static PyObject*
bb_str(PyBitBoard* self) {
    PyObject* class_name_obj = PyObject_GetAttrString((PyObject*)Py_TYPE(self), "__name__");
    if (!class_name_obj) {
        return PyUnicode_FromString("<UnknownClass>");
    }

    unsigned long long value = PyLong_AsUnsignedLongLong((PyObject*)self);
    if (PyErr_Occurred()) {
        Py_DECREF(class_name_obj);
        return NULL;
    }

    PyObject* result = PyUnicode_FromFormat("%U(0x%x)", class_name_obj, value);
    Py_DECREF(class_name_obj);
    return result;
}

PyTypeObject PyBitBoardType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "nchess_core.BitBoard",
    .tp_doc = "BitBoard object (inherits from int)",
    .tp_basicsize = sizeof(PyBitBoard),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_str = (reprfunc)bb_str,
    .tp_repr = (reprfunc)bb_str,
    .tp_methods = bitboard_methods,
    .tp_iter = bb_iter,
};