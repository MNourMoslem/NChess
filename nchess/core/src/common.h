#ifndef NCHESS_CORE_COMMON_H
#define NCHESS_CORE_COMMON_H

#include "nchess/nchess.h"
#include "nchess/utils.h"
#include "nchess/move.h"
#include "pymove.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

// Compatibility for Python < 3.10
#ifndef Py_IsNone
#define Py_IsNone(x) ((x) == Py_None)
#endif

#define CHECK_NO_SQUARE_ERR(sqr, return_type)\
if (sqr == NCH_NO_SQR){\
    if (!PyErr_Occurred()){\
        PyErr_SetString(\
            PyExc_ValueError,\
            "NO_SQUARE is invalid for this function"\
        );\
    }\
    return return_type;\
}

extern const char* MoveType2Str[];

NCH_STATIC_INLINE PyObject*
piece_to_pyobject(Piece p){
    return PyLong_FromLong(p);
}

NCH_STATIC_INLINE PyObject*
side_to_pyobject(Side s){
    return PyLong_FromLong(s);
}

NCH_STATIC_INLINE PyObject*
square_to_pyobject(Square s){
    return PyLong_FromLong(s);
}

Square
unicode_to_square(PyObject* uni);

Square
pyobject_as_square(PyObject* s);

Piece
pyobject_as_piece(PyObject* obj);

int
pyobject_as_move(PyObject* obj, Move* dst_move);

MoveType
pyobject_as_move_type(PyObject* obj);

Side
pyobject_as_side(PyObject* obj, int support_both_sides);

NCH_STATIC_INLINE PieceType
pyobject_as_piece_type(PyObject* obj){
    Piece p = pyobject_as_piece(obj);
    if (p == NCH_NO_PIECE)
        return NCH_NO_PIECE_TYPE;

    return Piece_TYPE(p);
}

#endif // NCHESS_CORE_COMMON_H