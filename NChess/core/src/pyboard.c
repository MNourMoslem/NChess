#include "pyboard.h"
#include "nchess/nchess.h"
#include "nchess/utils.h"
#include <stdio.h>
#include <numpy/arrayobject.h>
#include "common.h"
#include "pymove.h"
#include "bb_functions.h"
#include "array_conversion.h"
#include "nchess/generate_utils.h"

#define board(pyb) ((PyBoard*)pyb)->board

PyObject*
moves_to_list(Move* moves, int nmoves){
    PyObject* list = PyList_New(nmoves);
    PyMove* pymove;

    for (int i = 0; i < nmoves; i++){
        pymove = PyMove_New(moves[i]);
        if (!pymove){
            Py_DECREF(list);
            return NULL;
        }

        PyList_SetItem(list, i, pymove);
    }

    return list;
}

PyObject*
PyBoard_New(PyTypeObject *self, PyObject *args, PyObject *kwargs){
    char* fen = NULL;
    static char* kwlist[] = {"fen", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|s", kwlist, &fen)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError ,"failed reading the argmuents");
        }
        return NULL;
    }

    PyBoard* pyb = PyObject_New(PyBoard, &PyBoardType);
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
board__makemove(PyObject* self, PyObject* args){
    PyObject* step;

    if (!PyArg_ParseTuple(args, "O", &step)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the step argument");
        }
        return NULL;
    }

    Move move = pyobject_as_move(step);
    if (!move)
        return NULL;

    _Board_MakeMove(board(self), move);

    Py_RETURN_NONE;
}

PyObject*
board_step(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* step;
    static char* kwlist[] = {"step", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &step)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the step argument");
        }
        return NULL;
    }

    Move move = pyobject_as_move(step);
    if (!move)
        return NULL;

    int out = Board_StepByMove(board(self), move);
    return PyBool_FromLong(out);
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
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    long long nmoves;
    if (no_print) {
        nmoves = Board_PerftNoPrint(board(self), deep);
    } else if (pretty) {
        nmoves = Board_PerftPretty(board(self), deep);
    } else {
        nmoves = Board_Perft(board(self), deep);
    }

    return PyLong_FromLongLong(nmoves);
}

PyObject*
board_generate_legal_moves(PyObject* self, PyObject* args, PyObject* kwargs){
    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(board(self), moves);

    PyObject* list = PyList_New(nmoves);
    PyMove* pymove;

    for (int i = 0; i < nmoves; i++){
        pymove = PyMove_New(moves[i]);
        if (!pymove){
            Py_DECREF(list);
            return NULL;
        }

        PyList_SetItem(list, i, pymove);
    }

    return list;
}

NCH_STATIC_INLINE void
board2tensor(Board* board, int* tensor, int reversed){
    int i = 0;
    for (Side s = 0; s < NCH_SIDES_NB; s++){
        for (Piece p = 0; p < NCH_PIECE_NB; p++){
            bb2array(board->bitboards[s][p], tensor + i * NCH_SQUARE_NB, reversed);
            i++;
        }
    }
}

static PyObject*
board_as_array(PyObject* self, PyObject* args, PyObject* kwargs) {
    int reversed = 0;
    int as_list = 0;

    int nitems = NCH_SIDES_NB * NCH_PIECE_NB * NCH_SQUARE_NB;
    npy_intp dims[NPY_MAXDIMS];
    int ndim = parse_board_conversion_function_args(nitems, dims, args, kwargs, &reversed, &as_list);

    if (ndim < 0){
        return NULL;
    }

    if (!ndim){
        ndim = 2;
        dims[0] = NCH_SIDES_NB * NCH_PIECE_NB;
        dims[1] = NCH_SQUARE_NB;
    }
    
    if (as_list){
        int data[NCH_SIDES_NB * NCH_PIECE_NB * NCH_SQUARE_NB];
        board2tensor(board(self), data, reversed);
        return create_list_array(data, dims, ndim);
    }
    else{
        int* data = (int*)malloc(nitems * sizeof(int));
        if (!data){
            PyErr_NoMemory();
            return NULL;
        }

        board2tensor(board(self), data, reversed);
        
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

NCH_STATIC_INLINE void
board2table(Board* board, int* table, int reversed){
    #define TABLE_ITEM(board, idx, default)\
    Board_WHITE_PIECE(board, idx) != NCH_NO_PIECE\
    ? Board_WHITE_PIECE(board, idx) + NCH_White * NCH_PIECE_NB\
    : Board_BLACK_PIECE(board, idx) != NCH_NO_PIECE\
    ? Board_BLACK_PIECE(board, idx) + NCH_Black * NCH_PIECE_NB\
    : default

    if (reversed){
        for (Square s = 0; s < NCH_SQUARE_NB; s++){
            table[NCH_SQUARE_NB - 1 - s] = TABLE_ITEM(board, s, NCH_PIECE_NB * NCH_SIDES_NB);
        }
    }
    else{
        for (Square s = 0; s < NCH_SQUARE_NB; s++){
            table[s] = TABLE_ITEM(board, s, NCH_PIECE_NB * NCH_SIDES_NB);
        }
    }    
}

static PyObject*
board_as_table(PyObject* self, PyObject* args, PyObject* kwargs) {
    int reversed = 0;
    int as_list = 0;

    int nitems = NCH_SQUARE_NB;
    npy_intp dims[NPY_MAXDIMS];
    int ndim = parse_board_conversion_function_args(nitems, dims, args, kwargs, &reversed, &as_list);

    if (ndim < 0){
        return NULL;
    }

    if (!ndim){
        ndim = 1;
        dims[0] = NCH_SQUARE_NB;
    }

    if (as_list){
        int data[NCH_SQUARE_NB];
        board2table(board(self), data, reversed);
        return create_list_array(data, dims, ndim);
    }
    else{
        int* data = (int*)malloc(nitems * sizeof(int));
        if (!data){
            PyErr_NoMemory();
            return NULL;
        }

        board2table(board(self), data, reversed);
        
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
board_on_square(PyObject* self, PyObject* args){
    PyObject* s;

    if (!PyArg_ParseTuple(args, "O", &s)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
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
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
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

PyObject*
board_get_played_moves(PyObject* self, PyObject* args){
    int nmoves = Board_NMOVES(board(self));

    PyObject* list = PyList_New(nmoves);
    PyMove* pymove;

    MoveList* ml = &board(self)->movelist;
    MoveNode* node;

    for (int i = 0; i < nmoves; i++){
        node = MoveList_Get(ml, i);
        if (!node){
            Py_DECREF(list);
            return NULL;
        }

        pymove = PyMove_New(node->move);
        if (!pymove){
            Py_DECREF(list);
            return NULL;
        }

        PyList_SetItem(list, i, pymove);
    }

    return list;
}

PyObject*
board_reset(PyObject* self, PyObject* args){
    Board_Reset(board(self));
    Py_RETURN_NONE;
}

PyObject*
board_is_check(PyObject* self, PyObject* args){
    return PyBool_FromLong(Board_IsCheck(board(self)));
}

PyObject*
board_is_insufficient_material(PyObject* self, PyObject* args){
    return PyBool_FromLong(Board_IsInsufficientMaterial(board(self)));
}

PyObject*
board_is_threefold(PyObject* self, PyObject* args){
    return PyBool_FromLong(Board_IsThreeFold(board(self)));
}

PyObject*
board_is_fifty_moves(PyObject* self, PyObject* args){
    return PyBool_FromLong(Board_IsFiftyMoves(board(self)));
}

PyObject*
board_get_attackers_map(PyObject* self, PyObject* args, PyObject* kwargs){
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
        return NULL;

    Board* b = board(self);
    Side side = Board_SIDE(b);
    uint64 all_occ = Board_ALL_OCC(b);

    return PyLong_FromUnsignedLongLong(get_checkmap(b, side, s, all_occ));
}

PyObject*
board_get_moves_of(PyObject* self, PyObject* args, PyObject* kwargs){
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
        return NULL;

    Board* b = board(self);
    Move moves[30];
    int nmoves = Board_GetMovesOf(b, s, moves);
    return moves_to_list(moves, nmoves);
}

PyObject*
board_copy(PyObject* self, PyObject* args){
    Board* src = board(self);
    Board* dst = Board_NewCopy(src);
    if (!dst){
        PyErr_NoMemory();
        return NULL;
    }

    PyBoard* pyb = PyObject_New(PyBoard, &PyBoardType);
    if (pyb == NULL) {
        PyErr_NoMemory();
        return NULL;
    }
    pyb->board = dst;
    return pyb;
}

PyObject*
board_state(PyObject* self, PyObject* args, PyObject* kwargs){
    int can_move;
    static char* kwlist[] = {"can_move", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "p", kwlist, &can_move)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    return PyLong_FromUnsignedLong(Board_State(board(self), can_move));
}

PyObject*
board_find(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* p_obj;
    static char* kwlist[] = {"piece", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &p_obj)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    Piece p = pyobject_as_piece(p_obj);
    if (p == NCH_NO_PIECE)
        return NULL;

    Side side = Board_SIDE(board(self));
    uint64 bb = board(self)->bitboards[side][p];

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

NCH_STATIC PyMethodDef methods[] = {
    {"_makemove",
     (PyCFunction)board__makemove,
      METH_VARARGS,
      NULL},

    
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

    {"as_table",
     (PyCFunction)board_as_table,
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

    {"get_played_moves",
     (PyCFunction)board_get_played_moves,
      METH_NOARGS,
      NULL},

    {"reset",
     (PyCFunction)board_reset,
      METH_NOARGS,
      NULL},

    {"is_check",
     (PyCFunction)board_is_check,
      METH_NOARGS,
      NULL},

    {"get_attackers_map",
     (PyCFunction)board_get_attackers_map,
      METH_VARARGS | METH_KEYWORDS,
      NULL},

    {"get_moves_of",
     (PyCFunction)board_get_moves_of,
      METH_VARARGS | METH_KEYWORDS,
      NULL},

    {"is_insufficient_material",
     (PyCFunction)board_is_insufficient_material,
      METH_NOARGS,
      NULL},

    {"is_threefold",
     (PyCFunction)board_is_threefold,
      METH_NOARGS,
      NULL},

    {"is_fifty_moves",
     (PyCFunction)board_is_fifty_moves,
      METH_NOARGS,
      NULL},

    {"copy",
     (PyCFunction)board_copy,
      METH_NOARGS,
      NULL},

    {"state",
     (PyCFunction)board_state,
      METH_VARARGS | METH_KEYWORDS,
      NULL},

    {"find",
     (PyCFunction)board_find,
      METH_VARARGS | METH_KEYWORDS,
      NULL},

    {NULL, NULL, 0, NULL},
};
PyObject*
board_get_white_pawns(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_WHITE_PAWNS(board(self)));
}

PyObject*
board_get_black_pawns(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_BLACK_PAWNS(board(self)));
}

PyObject*
board_get_white_knights(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_WHITE_KNIGHTS(board(self)));
}

PyObject*
board_get_black_knights(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_BLACK_KNIGHTS(board(self)));
}

PyObject*
board_get_white_bishops(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_WHITE_BISHOPS(board(self)));
}

PyObject*
board_get_black_bishops(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_BLACK_BISHOPS(board(self)));
}

PyObject*
board_get_white_rooks(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_WHITE_ROOKS(board(self)));
}

PyObject*
board_get_black_rooks(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_BLACK_ROOKS(board(self)));
}

PyObject*
board_get_white_queens(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_WHITE_QUEENS(board(self)));
}

PyObject*
board_get_black_queens(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_BLACK_QUEENS(board(self)));
}

PyObject*
board_get_white_king(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_WHITE_KING(board(self)));
}

PyObject*
board_get_black_king(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_BLACK_KING(board(self)));
}

PyObject*
board_get_white_occ(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_WHITE_OCC(board(self)));
}

PyObject*
board_get_black_occ(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_BLACK_OCC(board(self)));
}

PyObject*
board_get_all_occ(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_ALL_OCC(board(self)));
}

PyObject*
board_castles(PyObject* self, void* something){
    return PyLong_FromUnsignedLongLong(Board_CASTLES(board(self)));
}

PyObject*
board_castles_str(PyObject* self, void* something){
    uint8 castles = Board_CASTLES(board(self));
    if (!castles)
        Py_RETURN_NONE;

    char buffer[5];
    int i = 0;

    if (Board_IS_CASTLE_WK(board(self)))
        buffer[i++] = 'K';
    if (Board_IS_CASTLE_WQ(board(self)))
        buffer[i++] = 'Q';
    if (Board_IS_CASTLE_BK(board(self)))
        buffer[i++] = 'k';
    if (Board_IS_CASTLE_BQ(board(self)))
        buffer[i++] = 'q';
    buffer[i] = '\0';

    return PyUnicode_FromString(buffer);
}

PyObject*
board_nmoves(PyObject* self, void* something){
    return PyLong_FromLong(Board_NMOVES(board(self)));
}

PyObject*
board_fifty_counter(PyObject* self, void* something){
    return PyLong_FromLong(Board_FIFTY_COUNTER(board(self)));
}

PyObject*
board_en_passant_square(PyObject* self, void* something){
    Board* b = board(self);
    if (!b->en_passant_idx)
        Py_RETURN_NONE;

    return PyLong_FromLong(NCH_SQR(b->en_passant_trg));
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
    {"castles", (getter)board_castles, NULL, NULL, NULL},
    {"castles_str", (getter)board_castles_str, NULL, NULL, NULL},
    {"nmoves", (getter)board_nmoves, NULL, NULL, NULL},
    {"fifty_counter", (getter)board_fifty_counter, NULL, NULL, NULL},
    {"en_passant_sqr", (getter)board_en_passant_square, NULL, NULL, NULL},
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