#ifndef NCHESS_CORE_COMMON_H
#define NCHESS_CORE_COMMON_H

#include "src/nchess.h"
#include "src/utils.h"
#define PY_SSIZE_T_CLEAN
#include <Python.h>

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

NCH_STATIC_INLINE Square
unicode_to_square(PyObject* uni){
    const char* s_str = PyUnicode_AsUTF8(uni);
    if (s_str == NULL) {
        PyErr_SetString(PyExc_ValueError, "failed to convert square to string");
        return NCH_NO_SQR;
    }
    return str_to_square(s_str);
}

NCH_STATIC_INLINE Square
pyobject_as_square(PyObject* s){
    Square sqr;
    if (PyUnicode_Check(s)){
        sqr = unicode_to_square(s);
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

NCH_STATIC_INLINE Piece
pyobject_as_piece(PyObject* p){
    if (!PyLong_Check(p)){
        PyErr_Format(PyExc_ValueError,
        "piece expected to be an int. got %s",
        Py_TYPE(p)->tp_name);
        return NCH_NO_PIECE;
    }
    return (Piece)PyLong_AsUnsignedLong(p);
}

#endif // NCHESS_CORE_COMMON_H