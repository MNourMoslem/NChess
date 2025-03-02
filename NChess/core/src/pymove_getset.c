
#include "pymove_getset.h"
#include "nchess/move.h"
#include "nchess/utils.h"
#include "pymove.h"

PyObject*
get_from_sqr(PyObject* self, void* something){
    Move move = PyMove_AsMove(self);
    return PyLong_FromUnsignedLong(Move_FROM(move));
}

PyObject*
get_to_sqr(PyObject* self, void* something){
    Move move = PyMove_AsMove(self);
    return PyLong_FromUnsignedLong(Move_TO(move));
}

PyObject*
get_pro_piece(PyObject* self, void* something){
    Move move = PyMove_AsMove(self);
    if (Move_IsPromotion(move)){
        return PyLong_FromUnsignedLong(Move_PRO_PIECE(move));
    }
    return PyLong_FromLong(0);
}

PyObject*
get_move_type(PyObject* self, void* something){
    Move move = PyMove_AsMove(self);
    return PyLong_FromUnsignedLong(Move_TYPE(move));
}

PyObject*
get_is_normal(PyObject* self, void* something){
    Move move = PyMove_AsMove(self);
    return PyBool_FromLong(Move_IsNormal(move));
}

PyObject*
get_is_enpassant(PyObject* self, void* something){
    Move move = PyMove_AsMove(self);
    return PyBool_FromLong(Move_IsEnPassant(move));
}

PyObject*
get_is_castle(PyObject* self, void* something){
    Move move = PyMove_AsMove(self);
    return PyBool_FromLong(Move_IsCastle(move));
}

PyObject*
get_is_promotion(PyObject* self, void* something){
    Move move = PyMove_AsMove(self);
    return PyBool_FromLong(Move_IsPromotion(move));
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
    {NULL          , NULL                     , NULL, NULL, NULL},
};
