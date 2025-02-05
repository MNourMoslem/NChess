#include "pyboard.h"
#include "nchess/nchess.h"
#include <numpy/arrayobject.h>
#include "common.h"
#include <stdio.h>
#include "pymove.h"
#include "bb_functions.h"
#include "bb_module.h"

PyObject*
uci_as_square(PyObject* self, PyObject* args){
    PyObject* uni;
    if (!PyArg_ParseTuple(args, "O", &uni)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the arguments to get the square from uci");
        return NULL;
    }

    if (!PyUnicode_Check(uni)){
        PyErr_Format(PyExc_ValueError,
         "square expected to be a string. got %s",
         Py_TYPE(uni)->tp_name);
        return NULL;
    }

    const char* s_str = PyUnicode_AsUTF8(uni);
    if (s_str == NULL) {
        PyErr_SetString(PyExc_ValueError, "failed to string to square");
        return NULL;
    }
    return square_to_pyobject(str_to_square(s_str));
}

PyObject*
square_file(PyObject* self, PyObject* args){
    PyObject* s;
    if (!PyArg_ParseTuple(args, "O", &s)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the arguments to get the file of a square");
        return NULL;
    }

    Square sqr = pyobject_as_square(s);
    if (!is_valid_square(sqr)){
        if (PyErr_Occurred())
            return NULL;

        Py_RETURN_NONE;
    }

    return PyLong_FromLong(NCH_GET_COLIDX(sqr));
}

PyObject*
square_rank(PyObject* self, PyObject* args){
    PyObject* s;
    if (!PyArg_ParseTuple(args, "O", &s)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the arguments to get the rank of a square");
        return NULL;
    }

    Square sqr = pyobject_as_square(s);
    if (!is_valid_square(sqr)){
        if (PyErr_Occurred())
            return NULL;

        Py_RETURN_NONE;
    }

    return PyLong_FromLong(NCH_GET_ROWIDX(sqr));
}

PyObject*
square_distance(PyObject* self, PyObject* args){
    PyObject* s1, *s2;
    if (!PyArg_ParseTuple(args, "OO", &s1, &s2)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the arguments to calculate the distance between two squares");
        return NULL;
    }

    Square sqr1 = pyobject_as_square(s1);
    Square sqr2 = pyobject_as_square(s2);
    if (!is_valid_square(sqr1) || !is_valid_square(sqr2)){
        if (PyErr_Occurred())
            return NULL;

        Py_RETURN_NONE;
    }

    return PyLong_FromLong(NCH_SQR_DISTANCE(sqr1, sqr2));
}

PyObject*
square_mirror(PyObject* self, PyObject* args){
    PyObject* s;
    int is_verical = 1;
    if (!PyArg_ParseTuple(args, "O|p", &s)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the arguments to mirror a square");
        return NULL;
    }

    Square sqr = pyobject_as_square(s);
    if (!is_valid_square(sqr)){
        if (PyErr_Occurred())
            return NULL;

        Py_RETURN_NONE;
    }

    return square_to_pyobject(is_verical ? NCH_SQR_MIRROR_V(sqr) : NCH_SQR_MIRROR_V(sqr));
}

PyObject*
_bb_as_list(PyObject* self, PyObject* args, PyObject* kwargs){
    uint64 bb;
    int reversed = 0;

    static const* kwlist[] = {"bb", "reversed", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "K|i", kwlist, &bb, &reversed)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    int arr[NCH_SQUARE_NB];
    bb2array(bb, arr, reversed);

    PyObject* list = PyList_New(NCH_SQUARE_NB);
    if (!list){
        PyErr_NoMemory();
        return NULL;
    }

    PyObject* item;
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        item = PyLong_FromLong(arr[i]);
        if (!item){
            PyErr_SetString(PyExc_ValueError, "failed to convert the items to python int");
            Py_DECREF(list);
            return NULL;
        }
        
        PyList_SetItem(list, i, item);
    }
    
    return list;
}


PyObject*
_bb_as_array(PyObject* self, PyObject* args, PyObject* kwargs){
    uint64 bb;
    int reversed = 0;

    static const* kwlist[] = {"bb", "reversed", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "K|i", kwlist, &bb, &reversed)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    int arr[NCH_SQUARE_NB];
    bb2array(bb, arr, reversed);

    PyObject* list = PyList_New(NCH_SQUARE_NB);
    if (!list){
        PyErr_NoMemory();
        return NULL;
    }

    PyObject* item;
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        item = PyLong_FromLong(arr[i]);
        if (!item){
            PyErr_SetString(PyExc_ValueError, "failed to convert the items to python int");
            Py_DECREF(list);
            return NULL;
        }
        
        PyList_SetItem(list, i, item);
    }
    
    return list;
}

// Method definitions
static PyMethodDef nchess_methods[] = {
    {"uci_as_square", (PyCFunction)uci_as_square, METH_VARARGS, NULL},
    {"square_file", (PyCFunction)square_file, METH_VARARGS, NULL},
    {"square_rank", (PyCFunction)square_rank, METH_VARARGS, NULL},
    {"square_distance", (PyCFunction)square_distance, METH_VARARGS, NULL},
    {"square_mirror", (PyCFunction)square_mirror, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}  // Sentinel
};

static PyModuleDef nchess = {
    PyModuleDef_HEAD_INIT,
    .m_name = "nchess",
    .m_doc = "NChess module",
    .m_size = -1,
    .m_methods = &nchess_methods
};

// Initialize the module
PyMODINIT_FUNC PyInit_nchess(void) {
    PyObject* m;

    // Initialize PyBoardType
    if (PyType_Ready(&PyBoardType) < 0) {
        return NULL;
    }

    if (PyType_Ready(&PyMoveType) < 0) {
        return NULL;
    }

    // Create the module
    m = PyModule_Create(&nchess);
    if (m == NULL) {
        return NULL;
    }

    PyObject *bb = PyInit_bb_module();
    if (bb == NULL) {
        Py_DECREF(m);
        return NULL;
    }

    if (PyModule_AddObject(m, "bb", bb) < 0) {
        Py_DECREF(bb);
        Py_DECREF(m);
        return NULL;
    }

    // Add PyBoardType to the module
    Py_INCREF(&PyBoardType);
    if (PyModule_AddObject(m, "Board", (PyObject*)&PyBoardType) < 0) {
        Py_DECREF(&PyBoardType);
        Py_DECREF(bb);
        Py_DECREF(m);
        return NULL;
    }

    // Add PyMoveType to the module
    Py_INCREF(&PyMoveType);
    if (PyModule_AddObject(m, "Move", (PyObject*)&PyMoveType) < 0) {
        Py_DECREF(&PyMoveType);
        Py_DECREF(bb);
        Py_DECREF(m);
        return NULL;
    }

    // Initialize additional components
    NCH_Init();

    return m;
}

