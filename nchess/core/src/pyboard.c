#include "pyboard.h"
#include "pyboard_getset.h"
#include "pyboard_methods.h"
#include "common.h"

#include "nchess/fen.h"
#include "nchess/io.h"

#define PY_SSIZE_CLEAN_H
#include <Python.h>

PyObject*
PyBoard_FromBoardWithType(PyTypeObject *self, Board* board){
    PyBoard* pyb = (PyBoard*)self->tp_alloc(self, 0);
    if (pyb == NULL) {
        PyErr_NoMemory();
        return NULL;
    }

    pyb->board = board;
    return (PyObject*)pyb;
}

PyObject*
PyBoard_FromBoard(Board* board){
    return PyBoard_FromBoardWithType(&PyBoardType, board);
}

PyObject*
board_new(PyTypeObject *self, PyObject *args, PyObject *kwargs){
    PyObject* fen_obj = NULL;
    static char* kwlist[] = {"fen", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O", kwlist, &fen_obj)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError ,"failed reading the argmuents");
        }
        return NULL;
    }

    Board* board = NULL;
    if (fen_obj && !Py_IsNone(fen_obj)){
        if (!PyUnicode_Check(fen_obj)){
            PyErr_Format(
                PyExc_TypeError,
                "fen must be string. got %s",
                Py_TYPE(fen_obj)->tp_name
            );
            return NULL;
        }
        
        const char* fen = PyUnicode_AsUTF8(fen_obj);
        if (PyErr_Occurred()){
            return NULL;
        }

        board = Board_NewFen(fen);
        if (!board){
            PyErr_SetString(PyExc_ValueError ,"could not read the fen");
            return NULL;
        }
    }
    else{
        board = Board_New();    
        if (!board){
            PyErr_NoMemory();
            return NULL;
        }
    }

    return PyBoard_FromBoardWithType(self, board);
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
    .tp_repr = (reprfunc)board_str,
    .tp_methods = pyboard_methods,
    .tp_getset = pyboard_getset,
};