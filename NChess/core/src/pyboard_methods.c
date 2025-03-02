#include "pyboard_methods.h"
#include "nchess/nchess.h"
#include "pymove.h"
#include "pybb.h"
#include "common.h"
#include "array_conversion.h"
#include "pyboard.h"
#include "bb_functions.h"

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>


#define BOARD(pyb) ((PyBoard*)pyb)->board

PyObject*
moves_to_list(Move* moves, int nmoves){
    PyObject* list = PyList_New(nmoves);
    PyObject* pymove;

    for (int i = 0; i < nmoves; i++){
        pymove = (PyObject*)PyMove_FromMove(moves[i]);
        if (!pymove){
            Py_DECREF(list);
            return NULL;
        }
        
        PyList_SetItem(list, i, pymove);
    }

    return list;
}

PyObject*
moves_to_set(Move* moves, int nmoves) {
    PyObject* set = PySet_New(NULL);
    if (!set) return NULL;

    PyMove* pymove;

    for (int i = 0; i < nmoves; i++) {
        pymove = PyMove_FromMove(moves[i]);
        if (!pymove) {
            Py_DECREF(set);
            return NULL;
        }

        if (PySet_Add(set, (PyObject*)pymove) < 0) {
            Py_DECREF(pymove);
            Py_DECREF(set);
            return NULL;
        }

        Py_DECREF(pymove);
    }

    return set;
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

    Move move;
    if (!pyobject_as_move(step, &move)){
        return NULL;
    }

    _Board_MakeMove(BOARD(self), move);

    Py_RETURN_NONE;
}

PyObject*
board_step(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* move_obj;
    static char* kwlist[] = {"move", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &move_obj)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the step argument");
        }
        return NULL;
    }

    Move move;
    if (!pyobject_as_move(move_obj, &move)){
        return NULL;
    }


    int out = Board_StepByMove(BOARD(self), move);
    return PyBool_FromLong(out);
}

PyObject*
board_undo(PyObject* self){
    Board_Undo(BOARD(self));
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
        nmoves = Board_PerftNoPrint(BOARD(self), deep);
    } else if (pretty) {
        nmoves = Board_PerftPretty(BOARD(self), deep);
    } else {
        nmoves = Board_Perft(BOARD(self), deep);
    }

    return PyLong_FromLongLong(nmoves);
}

PyObject*
board_generate_legal_moves(PyObject* self, PyObject* args, PyObject* kwargs){
    int as_set = 0;
    NCH_STATIC char* kwlist[] = {"as_set", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|p", kwlist, &as_set)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    Move moves[256];
    int nmoves = Board_GenerateLegalMoves(BOARD(self), moves);

    if (as_set){
        return moves_to_set(moves, nmoves);
    }
    return moves_to_list(moves, nmoves);
}

NCH_STATIC_INLINE void
board2tensor(Board* board, int* tensor, int reversed){
    for (Piece p = NCH_WPawn; p < NCH_PIECE_NB; p++){
        bb2array(Board_BB(board, p), tensor, reversed);
        tensor += NCH_SQUARE_NB;
    }
}

static PyObject*
board_as_array(PyObject* self, PyObject* args, PyObject* kwargs) {
    int reversed = 0;
    int as_list = 0;

    const int nitems = (NCH_PIECE_NB - 1) * NCH_SQUARE_NB;
    npy_intp dims[NPY_MAXDIMS];
    int ndim = parse_array_conversion_function_args(nitems, dims, args, kwargs, &reversed, &as_list);

    if (ndim < 0){
        return NULL;
    }

    if (!ndim){
        ndim = 2;
        dims[0] = (NCH_PIECE_NB - 1);
        dims[1] = NCH_SQUARE_NB;
    }
    
    if (as_list){
        int data[(NCH_PIECE_NB - 1) * NCH_SQUARE_NB];
        board2tensor(BOARD(self), data, reversed);
        return create_list_array(data, dims, ndim);
    }
    else{
        int* data = (int*)malloc(nitems * sizeof(int));
        if (!data){
            PyErr_NoMemory();
            return NULL;
        }

        board2tensor(BOARD(self), data, reversed);
        
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
    if (reversed){
        for (Square s = 0; s < NCH_SQUARE_NB; s++){
            table[NCH_SQUARE_NB - 1 - s] = Board_ON_SQUARE(board, s);
        }
    }
    else{
        for (Square s = 0; s < NCH_SQUARE_NB; s++){
            table[s] = Board_ON_SQUARE(board, s);
        }
    }    
}

static PyObject*
board_as_table(PyObject* self, PyObject* args, PyObject* kwargs) {
    int reversed = 0;
    int as_list = 0;

    int nitems = NCH_SQUARE_NB;
    npy_intp dims[NPY_MAXDIMS];
    int ndim = parse_array_conversion_function_args(nitems, dims, args, kwargs, &reversed, &as_list);

    if (ndim < 0){
        return NULL;
    }

    if (!ndim){
        ndim = 1;
        dims[0] = NCH_SQUARE_NB;
    }

    if (as_list){
        int data[NCH_SQUARE_NB];
        board2table(BOARD(self), data, reversed);
        return create_list_array(data, dims, ndim);
    }
    else{
        int* data = (int*)malloc(nitems * sizeof(int));
        if (!data){
            PyErr_NoMemory();
            return NULL;
        }

        board2table(BOARD(self), data, reversed);
        
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
    CHECK_NO_SQUARE_ERR(sqr, NULL)

    Piece p = Board_ON_SQUARE(BOARD(self), sqr);
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
    CHECK_NO_SQUARE_ERR(sqr, NULL)

    Side side = Board_OWNED_BY(BOARD(self), sqr);
    return side_to_pyobject(side);
}

PyObject*
board_get_played_moves(PyObject* self, PyObject* args){
    int nmoves = Board_NMOVES(BOARD(self));

    PyObject* list = PyList_New(nmoves);
    PyMove* pymove;

    MoveList* ml = &BOARD(self)->movelist;
    MoveNode* node;

    for (int i = 0; i < nmoves; i++){
        node = MoveList_Get(ml, i);
        if (!node){
            Py_DECREF(list);
            return NULL;
        }

        pymove = PyMove_FromMove(node->move);
        if (!pymove){
            Py_DECREF(list);
            return NULL;
        }

        PyList_SetItem(list, i, (PyObject*)pymove);
    }

    return list;
}

PyObject*
board_reset(PyObject* self, PyObject* args){
    Board_Reset(BOARD(self));
    Py_RETURN_NONE;
}

PyObject*
board_get_attackers_map(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* sqr;
    PyObject* side_obj = NULL;
    static char* kwlist[] = {"square", "attacker_side", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O", kwlist, &sqr, &side_obj)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }
    Board* b = BOARD(self);
    Square s = pyobject_as_square(sqr);
    CHECK_NO_SQUARE_ERR(s, NULL)

    Side side;
    if (side_obj && !Py_IsNone(side_obj)){
        side = pyobject_as_side(side_obj);
        if (PyErr_Occurred())
            return NULL;
    
        if (side == NCH_NO_SIDE){
            side = Board_SIDE(b);
        }
        else{
            side = NCH_OP_SIDE(side);
        }
    }
    else{
        side = Board_SIDE(b);
    }
    
    uint64 all_occ = Board_ALL_OCC(b);

    uint64 attack_map = get_checkmap(b, side, s, all_occ);

    return (PyObject*)PyBitBoard_FromUnsignedLongLong(attack_map);
}

PyObject*
board_get_moves_of(PyObject* self, PyObject* args, PyObject* kwargs){
    int as_set = 0;
    PyObject* sqr;
    static char* kwlist[] = {"square", "as_set", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|p", kwlist, &sqr, &as_set)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    Square s = pyobject_as_square(sqr);
    CHECK_NO_SQUARE_ERR(s, NULL)

    Board* b = BOARD(self);
    Move moves[30];
    int n = Board_GetMovesOf(b, s, moves);

    if (as_set){
        return moves_to_set(moves, n);
    }

    return moves_to_list(moves, n);
}

PyObject*
board_copy(PyObject* self, PyObject* args){
    Board* src = BOARD(self);
    Board* dst = Board_NewCopy(src);
    if (!dst){
        PyErr_NoMemory();
        return NULL;
    }

    PyBoard* pyb = (PyBoard*)Py_TYPE(self)->tp_alloc(Py_TYPE(self), 0);
    if (pyb == NULL) {
        PyErr_NoMemory();
        Board_Free(dst);
        return NULL;
    }
    pyb->board = dst;
    return (PyObject*)pyb;
}

PyObject*
board_get_game_state(PyObject* self, PyObject* args, PyObject* kwargs){
    int can_move = -1;
    static char* kwlist[] = {"can_move", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|p", kwlist, &can_move)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    can_move = can_move != -1 ? can_move : Board_CanMove(BOARD(self));
    GameState state = Board_State(BOARD(self), can_move);
    return PyLong_FromUnsignedLong(state);
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
    if (PyErr_Occurred())
        return NULL;

    uint64 bb = Board_BB(BOARD(self), p);
    
    PyObject* list = PyList_New(count_bits(bb));
    if (!list){
        PyErr_SetString(PyExc_ValueError, "failed to create a list");
        return NULL;
    }

    PyObject* idx_obj;
    int idx;
    Py_ssize_t i = 0;
    LOOP_U64_T(bb){
        idx_obj = PyLong_FromLong(idx);
        PyList_SetItem(list, i++, idx_obj);
    }

    return list;
}

PyObject*
board_fen(PyObject* self, PyObject* args, PyObject* kwargs){
    char buffer[400];
    Board_AsFen(BOARD(self), buffer);
    PyObject* str = PyUnicode_FromString(buffer);
    return str;
}

PyObject*
board_get_occ(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* side_obj;
    NCH_STATIC char* kwlist[] = {"side", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &side_obj)){
        return NULL;
    }

    Side side = pyobject_as_side(side_obj);
    if (side == NCH_NO_SIDE){
        if (!PyErr_Occurred()){
            PyErr_SetString(
                PyExc_ValueError,
                "side allowed are only 0 for white and 1 for black."
            );
        }
        return NULL;
    }

    uint64 bb = Board_OCC(BOARD(self), side);
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(bb);
}

PyObject*
board_is_move_legal(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* move_obj;
    NCH_STATIC char* kwlist[] = {"move", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &move_obj)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the move argument");
        }
        return NULL;
    }

    Move move;
    if (!pyobject_as_move(move_obj, &move)){
        return NULL;
    }

    if (Board_CheckAndMakeMoveLegal(BOARD(self), &move)) {Py_RETURN_TRUE;} Py_RETURN_FALSE;
}

PyMethodDef pyboard_methods[] = {
    {"undo"                    , (PyCFunction)board_undo                    , METH_NOARGS                  , NULL},
    {"get_played_moves"        , (PyCFunction)board_get_played_moves        , METH_NOARGS                  , NULL},
    {"reset"                   , (PyCFunction)board_reset                   , METH_NOARGS                  , NULL},
    {"copy"                    , (PyCFunction)board_copy                    , METH_NOARGS                  , NULL},
    {"fen"                     , (PyCFunction)board_fen                     , METH_NOARGS                  , NULL},

    {"_makemove"               , (PyCFunction)board__makemove               , METH_VARARGS                 , NULL},
    {"on_square"               , (PyCFunction)board_on_square               , METH_VARARGS                 , NULL},
    {"owned_by"                , (PyCFunction)board_owned_by                , METH_VARARGS                 , NULL},
    
    {"step"                    , (PyCFunction)board_step                    , METH_VARARGS | METH_KEYWORDS , NULL},
    {"perft"                   , (PyCFunction)board_perft                   , METH_VARARGS | METH_KEYWORDS , NULL},
    {"generate_legal_moves"    , (PyCFunction)board_generate_legal_moves    , METH_VARARGS | METH_KEYWORDS , NULL},
    {"as_array"                , (PyCFunction)board_as_array                , METH_VARARGS | METH_KEYWORDS , NULL},
    {"as_table"                , (PyCFunction)board_as_table                , METH_VARARGS | METH_KEYWORDS , NULL},
    {"get_attackers_map"       , (PyCFunction)board_get_attackers_map       , METH_VARARGS | METH_KEYWORDS , NULL},
    {"get_moves_of"            , (PyCFunction)board_get_moves_of            , METH_VARARGS | METH_KEYWORDS , NULL},
    {"get_game_state"          , (PyCFunction)board_get_game_state          , METH_VARARGS | METH_KEYWORDS , NULL},
    {"get_occ"                 , (PyCFunction)board_get_occ                 , METH_VARARGS | METH_KEYWORDS , NULL},
    {"find"                    , (PyCFunction)board_find                    , METH_VARARGS | METH_KEYWORDS , NULL},
    {"is_move_legal"           , (PyCFunction)board_is_move_legal           , METH_VARARGS | METH_KEYWORDS , NULL},

    {NULL                      , NULL                                       , 0                            , NULL},
};