#include "pymove.h"
#include "common.h"
#include "nchess/nchess.h"

PyMove*
PyMove_New(Square from_, Square to_, Piece promote, MoveType type){
    Move move = Move_New(from_, to_, promote, type);
    if (move == Move_NULL){
        PyErr_SetString(PyExc_ValueError, "invalid move");
        return NULL;
    }

    return (PyMove*)PyObject_CallFunction((PyObject*)&PyMoveType, "K", move);
}

PyMove*
PyMove_FromMove(Move move){
    return (PyMove*)PyObject_CallFunction((PyObject*)&PyMoveType, "K", move);
}

PyObject*
PyMove_Str(PyObject* self){
    char buffer[10];
    Move move = (Move)PyLong_AsUnsignedLong(self);
    Move_AsString(move, buffer);
    return PyUnicode_FromFormat("%s(\"%s\")", Py_TYPE(self)->tp_name, buffer);
}

PyObject*
get_from_sqr(PyObject* self, void* something){
    Move move = (Move)PyLong_AsUnsignedLong(self);
    return PyLong_FromUnsignedLong(Move_FROM(move));
}

PyObject*
get_to_sqr(PyObject* self, void* something){
    Move move = (Move)PyLong_AsUnsignedLong(self);
    return PyLong_FromUnsignedLong(Move_TO(move));
}

PyObject*
get_promote_piece(PyObject* self, void* something){
    Move move = (Move)PyLong_AsUnsignedLong(self);
    return PyLong_FromUnsignedLong(Move_PRO_PIECE(move));
}

PyObject*
get_move_type(PyObject* self, void* something){
    Move move = (Move)PyLong_AsUnsignedLong(self);
    return PyLong_FromUnsignedLong(Move_TYPE(move));
}

PyObject*
get_is_enpassant(PyObject* self, void* something){
    Move move = (Move)PyLong_AsUnsignedLong(self);
    return PyBool_FromLong(Move_IsEnPassant(move));
}

PyObject*
get_is_castle(PyObject* self, void* something){
    Move move = (Move)PyLong_AsUnsignedLong(self);
    return PyBool_FromLong(Move_IsCastle(move));
}

PyObject*
get_is_promotion(PyObject* self, void* something){
    Move move = (Move)PyLong_AsUnsignedLong(self);
    return PyBool_FromLong(Move_IsPromotion(move));
}

PyGetSetDef getset_methods[] = {
    {"from_"       , (getter)get_from_sqr     , NULL, NULL, NULL},
    {"to_"         , (getter)get_to_sqr       , NULL, NULL, NULL},
    {"promote"     , (getter)get_promote_piece, NULL, NULL, NULL},
    {"move_type"   , (getter)get_move_type    , NULL, NULL, NULL},
    {"is_enpassant", (getter)get_is_enpassant , NULL, NULL, NULL},
    {"is_castle"   , (getter)get_is_castle    , NULL, NULL, NULL},
    {"is_promotion", (getter)get_is_promotion , NULL, NULL, NULL},
};

PyTypeObject PyMoveType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Move",
    .tp_doc = "Move object",
    .tp_basicsize = sizeof(PyMove),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_str = PyMove_Str,
    .tp_repr = PyMove_Str,
    .tp_getset = getset_methods,
};