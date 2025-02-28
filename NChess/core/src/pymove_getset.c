
#include "pymove_getset.h"
#include "nchess/move.h"
#include "nchess/utils.h"

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
get_pro_piece(PyObject* self, void* something){
    Move move = (Move)PyLong_AsUnsignedLong(self);
    return PyLong_FromUnsignedLong(Move_PRO_PIECE(move));
}

PyObject*
get_move_type(PyObject* self, void* something){
    Move move = (Move)PyLong_AsUnsignedLong(self);
    return PyLong_FromUnsignedLong(Move_TYPE(move));
}

PyObject*
get_is_normal(PyObject* self, void* something){
    Move move = (Move)PyLong_AsUnsignedLong(self);
    return PyBool_FromLong(Move_IsNormal(move));
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

PyObject*
get_is_valid(PyObject* self, void* something){
    Move move = (Move)PyLong_AsUnsignedLong(self);
    Square from_ = Move_FROM(move);
    Square to_ = Move_TO(move);

    if (!is_valid_square(from_) || !is_valid_square(to_))
        Py_RETURN_FALSE;
    Py_RETURN_TRUE;
}


PyGetSetDef pymove_getset[] = {
    {"from_"       , (getter)get_from_sqr     , NULL, NULL, NULL},
    {"to_"         , (getter)get_to_sqr       , NULL, NULL, NULL},
    {"pro_piece"   , (getter)get_pro_piece    , NULL, NULL, NULL},
    {"move_type"   , (getter)get_move_type    , NULL, NULL, NULL},
    {"is_normal"   , (getter)get_is_normal    , NULL, NULL, NULL},
    {"is_enpassant", (getter)get_is_enpassant , NULL, NULL, NULL},
    {"is_castle"   , (getter)get_is_castle    , NULL, NULL, NULL},
    {"is_promotion", (getter)get_is_promotion , NULL, NULL, NULL},
    {"is_valid"    , (getter)get_is_valid     , NULL, NULL, NULL},
    {NULL          , NULL                     , NULL, NULL, NULL},
};
