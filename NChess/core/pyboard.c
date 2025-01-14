#include "pyboard.h"
#include "nchess.h"
#include "src/utils.h"
#include <stdio.h>
#include <numpy/arrayobject.h>

#define board(pyb) ((PyBoard*)pyb)->board

NCH_STATIC_INLINE PyObject*
piece_to_pyobject(Piece p){
    return PyLong_FromUnsignedLong(p);
}

NCH_STATIC_INLINE PyObject*
side_to_pyobject(Side s){
    return PyLong_FromUnsignedLong(s);
}


NCH_STATIC_INLINE Square
pyobject_as_square(PyObject* s){
    Square sqr;
    if (PyUnicode_Check(s)){
        const char* s_str = PyUnicode_AsUTF8(s);
        if (s_str == NULL) {
            PyErr_SetString(PyExc_ValueError, "failed to convert square to string");
            return NCH_NO_SQR;
        }
        sqr = str_to_square(s_str);
    }
    else if (PyLong_Check(s)){
        sqr = (Square)PyLong_AsUnsignedLong(s);
    }
    else{
        PyErr_Format(PyExc_ValueError,
        "square expected to be int or a string represents the square. got %i",
        Py_TYPE(s)->tp_name);
        return NCH_NO_SQR;
    }
    return sqr;
}

PyObject*
PyBoard_New(PyTypeObject *self, PyObject *args, PyObject *kwargs){
    char* fen = NULL;
    static char* kwlist[] = {"fen", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|s", kwlist, &fen)){
        PyErr_SetString(PyExc_ValueError ,"failed reading the argmuents");
        return NULL;
    }

    PyBoard* pyb = PyObject_New(PyBoard, &PyBoardType);
    if (pyb == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    if (fen){
        pyb->board = Board_FromFen(fen);
        if (!pyb->board){
            Py_DECREF(pyb);
            PyErr_SetString(PyExc_ValueError ,"could not read the fen");
            return NULL;
        }
        return pyb;
    }

    pyb->board = Board_New();

    if (!pyb->board){
        Py_DECREF(pyb);
        return NULL;
    }

    return pyb;
}

void
PyBoard_Free(PyObject* pyb){
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

PyObject*
board_step(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* step;
    static char* kwlist[] = {"step", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &step)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the step argument");
        return NULL;
    }

    if (PyUnicode_Check(step)) {
        const char* step_str = PyUnicode_AsUTF8(step);
        if (step_str == NULL) {
            PyErr_SetString(PyExc_ValueError, "failed to convert step to string");
            return NULL;
        }
        Board_Step(((PyBoard*)self)->board, step_str);
    } else if (PyLong_Check(step)) {
        Move step_move = PyLong_AsUnsignedLong(step);
        Board_StepByMove(board(self), step_move);
    } else {
        PyErr_Format(PyExc_TypeError, "step must be a string or an int, got %s", Py_TYPE(step)->tp_name);
        return NULL;
    }

    Py_RETURN_NONE;
}

PyObject*
board_undo(PyObject* self){
    Board_Undo(board(self));
    Py_RETURN_NONE;
}

PyObject*
board_perft(PyObject* self, PyObject* args, PyObject* kwargs){
    int deep;
    int pretty = 0;
    int no_print = 0;
    static char* kwlist[] = {"deep", "pretty", "no_print", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "i|pp", kwlist, &deep, &pretty, &no_print)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        return NULL;
    }

    long long nmoves;
    if (no_print) {
        nmoves = Board_PerftNoPrint(((PyBoard*)self)->board, deep);
    } else if (pretty) {
        nmoves = Board_PerftPretty(((PyBoard*)self)->board, deep);
    } else {
        nmoves = Board_Perft(((PyBoard*)self)->board, deep);
    }

    return PyLong_FromLongLong(nmoves);
}

PyObject*
board_generate_legal_moves(PyObject* self, PyObject* args, PyObject* kwargs){
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(board(self), moves);

    PyObject* list = PyList_New(nmoves);

    for (int i = 0; i < nmoves; i++){
        PyList_SetItem(list, i, PyLong_FromUnsignedLong(moves[i]));
    }

    return list;
}

NCH_STATIC void
bb2array(uint64 bb, int* arr, int reverse){
    // memset(arr, 0, sizeof(sizeof(int) * NCH_SQUARE_NB));
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        arr[i] = 0;
    }

    int idx;
    if (reverse){
        LOOP_U64_T(bb){
            arr[63 - idx] = 1;
        }
    }
    else{
        LOOP_U64_T(bb){
            arr[idx] = 1;
        }
    }

}


static PyObject*
board_as_array(PyObject* self, PyObject* args, PyObject* kwargs) {
    PyObject* shape = NULL;
    int reversed = 0;
    static char* kwlist[] = {"shape", "reversed", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|Op", kwlist, &shape, &reversed)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the shape argument");
        return NULL;
    }

    int nitems = NCH_SIDES_NB * NCH_PIECE_NB * NCH_SQUARE_NB;
    int ndim;
    npy_intp dims[NPY_MAXDIMS];
    if (shape && !Py_IsNone(shape)){
        if (!PySequence_Check(shape)){
            PyErr_Format(PyExc_TypeError,
             "shape expected to be a python sequence (list, tuple, ...). got %s",
             Py_TYPE(shape)->tp_name);

            return NULL;
        }

        ndim = PySequence_Length(shape);
        if (ndim > NPY_MAXDIMS){
            PyErr_Format(PyExc_ValueError,
             "could not create array from shape with ndim more then %i. got %i",
             NPY_MAXDIMS, ndim);
        
            return NULL;
        }

        PyObject* item;
        int total = 1;
        for (int i = 0; i < ndim; i++){
            item = PySequence_GetItem(shape, i);
            if (!item){
                PyErr_SetString(PyExc_ValueError, "failed getitem from the inputted shape");
                return NULL;
            }

            if (!PyNumber_Check(item)){
                PyErr_Format(PyExc_ValueError,
                "expected numbers as dimensions. got %s type",
                Py_TYPE(item)->tp_name);

                Py_DECREF(item);

                return NULL;
            }

            total *= PyLong_AsLong(PyNumber_Long(item));
            Py_DECREF(item);
        }

        if (total != nitems){
            PyErr_Format(PyExc_ValueError,
            "input shape expected to have %i number of items. got %i",
            nitems, total);

            return NULL;
        }

        for (int i = 0; i < ndim; i++){
            item = PySequence_GetItem(shape, i);
            dims[i] = PyLong_AsLong(PyNumber_Long(item));
            Py_DECREF(item);
        }
    }
    else{
        ndim = 2;
        dims[0] = NCH_SIDES_NB * NCH_PIECE_NB;
        dims[1] = NCH_SQUARE_NB;
    }

    import_array();

    int* tensor = (int*)malloc(nitems * sizeof(int));
    if (!tensor) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate memory for tensor");
        return NULL;
    }

    int i = 0;
    for (Side s = 0; s < NCH_SIDES_NB; s++){
        for (Piece p = 0; p < NCH_PIECE_NB; p++){
            bb2array(board(self)->bitboards[s][p], tensor + i * NCH_SQUARE_NB, reversed);
            i++;
        }
    }

    PyObject* numpy_array = PyArray_SimpleNewFromData(ndim, dims, NPY_INT, tensor);
    if (!numpy_array) {
        free(tensor); 
        PyErr_SetString(PyExc_RuntimeError, "Failed to create NumPy array");
        return NULL;
    }

    PyArray_ENABLEFLAGS((PyArrayObject*)numpy_array, NPY_ARRAY_OWNDATA);

    return numpy_array;
}

PyObject*
board_on_square(PyObject* self, PyObject* args){
    PyObject* s;

    if (!PyArg_ParseTuple(args, "O", &s)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        return NULL;
    }

    Square sqr = pyobject_as_square(s);
    if (!is_valid_square(sqr)){
        if (PyErr_Occurred())
            return NULL;

        Py_RETURN_NONE;
    }

    Piece p = Board_ON_SQUARE(board(self), sqr);
    return piece_to_pyobject(p);
}

PyObject*
board_owned_by(PyObject* self, PyObject* args){
    PyObject* s;

    if (!PyArg_ParseTuple(args, "O", &s)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        return NULL;
    }

    Square sqr = pyobject_as_square(s);
    if (!is_valid_square(sqr)){
        if (PyErr_Occurred())
            return NULL;

        Py_RETURN_NONE;
    }

    Side side = Board_OWNED_BY(board(self), sqr);
    return side_to_pyobject(side);
}

NCH_STATIC PyMethodDef methods[] = {
    {"step",
     (PyCFunction)board_step,
      METH_VARARGS | METH_KEYWORDS,
      NULL},

    {"undo",
     (PyCFunction)board_undo,  
     METH_NOARGS,
     NULL,
    },
    
    {"perft",
     (PyCFunction)board_perft,
      METH_VARARGS | METH_KEYWORDS,
      NULL},

    {"generate_legal_moves",
     (PyCFunction)board_generate_legal_moves,
      METH_NOARGS,
      NULL},

    {"as_array",
     (PyCFunction)board_as_array,
      METH_VARARGS | METH_KEYWORDS,
      NULL},

    {"on_square",
     (PyCFunction)board_on_square,
      METH_VARARGS,
      NULL},

    {"owned_by",
     (PyCFunction)board_owned_by,
      METH_VARARGS,
      NULL},

    {NULL, NULL, 0, NULL},
};
PyObject*
board_get_white_pawns(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_WHITE_PAWNS(((PyBoard*)self)->board));
}

PyObject*
board_get_black_pawns(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_BLACK_PAWNS(((PyBoard*)self)->board));
}

PyObject*
board_get_white_knights(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_WHITE_KNIGHTS(((PyBoard*)self)->board));
}

PyObject*
board_get_black_knights(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_BLACK_KNIGHTS(((PyBoard*)self)->board));
}

PyObject*
board_get_white_bishops(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_WHITE_BISHOPS(((PyBoard*)self)->board));
}

PyObject*
board_get_black_bishops(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_BLACK_BISHOPS(((PyBoard*)self)->board));
}

PyObject*
board_get_white_rooks(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_WHITE_ROOKS(((PyBoard*)self)->board));
}

PyObject*
board_get_black_rooks(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_BLACK_ROOKS(((PyBoard*)self)->board));
}

PyObject*
board_get_white_queens(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_WHITE_QUEENS(((PyBoard*)self)->board));
}

PyObject*
board_get_black_queens(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_BLACK_QUEENS(((PyBoard*)self)->board));
}

PyObject*
board_get_white_king(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_WHITE_KING(((PyBoard*)self)->board));
}

PyObject*
board_get_black_king(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_BLACK_KING(((PyBoard*)self)->board));
}

PyObject*
board_get_white_occ(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_WHITE_OCC(((PyBoard*)self)->board));
}

PyObject*
board_get_black_occ(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_BLACK_OCC(((PyBoard*)self)->board));
}

PyObject*
board_get_all_occ(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_ALL_OCC(((PyBoard*)self)->board));
}

NCH_STATIC PyGetSetDef getset[] = {
    {"white_pawns", (getter)board_get_white_pawns, NULL, NULL, NULL},
    {"black_pawns", (getter)board_get_black_pawns, NULL, NULL, NULL},
    {"white_knights", (getter)board_get_white_knights, NULL, NULL, NULL},
    {"black_knights", (getter)board_get_black_knights, NULL, NULL, NULL},
    {"white_bishops", (getter)board_get_white_bishops, NULL, NULL, NULL},
    {"black_bishops", (getter)board_get_black_bishops, NULL, NULL, NULL},
    {"white_rooks", (getter)board_get_white_rooks, NULL, NULL, NULL},
    {"black_rooks", (getter)board_get_black_rooks, NULL, NULL, NULL},
    {"white_queens", (getter)board_get_white_queens, NULL, NULL, NULL},
    {"black_queens", (getter)board_get_black_queens, NULL, NULL, NULL},
    {"white_king", (getter)board_get_white_king, NULL, NULL, NULL},
    {"black_king", (getter)board_get_black_king, NULL, NULL, NULL},
    {"white_occ", (getter)board_get_white_occ, NULL, NULL, NULL},
    {"black_occ", (getter)board_get_black_occ, NULL, NULL, NULL},
    {"all_occ", (getter)board_get_all_occ, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL}
};

PyTypeObject PyBoardType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Board",
    .tp_basicsize = sizeof(PyBoard),
    .tp_dealloc = (destructor)PyBoard_Free,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = (newfunc)PyBoard_New,
    .tp_str = (reprfunc)board_str,
    .tp_methods = &methods,
    .tp_getset = &getset
};