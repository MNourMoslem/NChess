#ifndef NCHESS_CORE_COMMON_H
#define NCHESS_CORE_COMMON_H

#include "nchess/nchess.h"
#include "nchess/utils.h"
#include "nchess/move.h"
#include "pymove.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#define CHECK_NO_SQUARE_ERR(sqr, return_type)\
if (sqr == NCH_NO_SQR){\
    if (!PyErr_Occurred()){\
        PyErr_SetString(\
            PyExc_ValueError,\
            "NO_SQURE is invalid for this function"\
        );\
    }\
    return return_type;\
}

#define CHECK_MOVE_NULL_ERR(move, return_type)\
if (move == Move_NULL){\
    if (!PyErr_Occurred()){\
        PyErr_SetString(\
            PyExc_ValueError,\
            "Move_NULL is invalid for this function"\
        );\
    }\
    return return_type;\
}

extern const char* Str2MoveType[];

NCH_STATIC_INLINE PyObject*
piece_to_pyobject(Piece p){
    return PyLong_FromUnsignedLong(p);
}

NCH_STATIC_INLINE PyObject*
side_to_pyobject(Side s){
    return PyLong_FromUnsignedLong(s);
}

NCH_STATIC_INLINE PyObject*
square_to_pyobject(Square s){
    return PyLong_FromUnsignedLong(s);
}

Square
unicode_to_square(PyObject* uni);

Square
pyobject_as_square(PyObject* s);

Piece
pyobject_as_piece(PyObject* obj);

Move
pyobject_as_move(PyObject* m);

MoveType
pyobject_as_move_type(PyObject* obj);

Side
pyobject_as_side(PyObject* obj);

NCH_STATIC_INLINE PieceType
pyobject_as_piece_type(PyObject* obj){
    Piece p = pyobject_as_piece(obj);
    return Piece_TYPE(p);
}

#endif // NCHESS_CORE_COMMON_H