#include "pyboard.h"
#include "pyboard_getset.h"
#include "pyboard_methods.h"

#include "nchess/fen.h"
#include "nchess/io.h"

#define PY_SSIZE_CLEAN_H
#include <Python.h>

PyObject*
board_new(PyTypeObject *self, PyObject *args, PyObject *kwargs){
    char* fen = NULL;
    static char* kwlist[] = {"fen", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|s", kwlist, &fen)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError ,"failed reading the argmuents");
        }
        return NULL;
    }

    PyBoard* pyb = (PyBoard*)self->tp_alloc(self, 0);
    if (pyb == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    
    if (fen){
        pyb->board = Board_NewFen(fen);
        if (!pyb->board){
            Py_DECREF(pyb);
            PyErr_SetString(PyExc_ValueError ,"could not read the fen");
            return NULL;
        }
    }
    else{
        pyb->board = Board_New();    
        if (!pyb->board){
            Py_DECREF(pyb);
            return NULL;
        }
    }

    return (PyObject*)pyb;
}

void
board_free(PyObject* pyb){
    if (pyb){
        PyBoard* b = (PyBoard*)pyb;
        Board_Free(b->board);
        Py_TYPE(b)->tp_free(b);
    }
}

PyObject*
board_str(PyObject* pyb){
    PyBoard* b = (PyBoard*)pyb;
    char buffer[100];
    Board_AsString(b->board, buffer);
    PyObject* str = PyUnicode_FromString(buffer);
    return str;
}

PyTypeObject PyBoardType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Board",
    .tp_basicsize = sizeof(PyBoard),
    .tp_dealloc = (destructor)board_free,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = (newfunc)board_new,
    .tp_str = (reprfunc)board_str,
    .tp_methods = pyboard_methods,
    .tp_getset = pyboard_getset,
};