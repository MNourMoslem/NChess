#include "common.h"

const char* MoveType2Str[] = {
    "normal", "promotion", "enpassant", "castle",
};

Square
unicode_to_square(PyObject* uni){
    const char* s_str = PyUnicode_AsUTF8(uni);
    if (s_str == NULL) {
        PyErr_SetString(PyExc_ValueError, "failed to convert square to string");
        return NCH_NO_SQR;
    }

    Square sqr = str_to_square(s_str);
    if (sqr == NCH_NO_SQR){
        PyErr_SetString(
            PyExc_ValueError,
            "invalid string to create a square"
        );
    }

    return sqr;
}

Square
pyobject_as_square(PyObject* s){
    Square sqr;
    if (PyUnicode_Check(s)){
        sqr = unicode_to_square(s);
    }
    else if (PyLong_Check(s)){
        sqr = (Square)PyLong_AsLongLong(s);
        if (!is_valid_square(sqr)){
            PyErr_Format(
                PyExc_ValueError,
                "square must be in range from 0 to 63. got %i",
                sqr
            );
            return NCH_NO_SQR;
        }
    }
    else{
        PyErr_Format(
            PyExc_ValueError,
            "square expected to be int or a string represents the square. got %i",
            Py_TYPE(s)->tp_name
        );
        return NCH_NO_SQR;
    }

    return sqr;
}

Piece
pyobject_as_piece(PyObject* obj){
    if (Py_IsNone(obj)){
        return NCH_NO_PIECE;
    }

    Piece p;
    if (PyLong_Check(obj)){
        p = PyLong_AsLong(obj);
        if (!is_valid_piece(p)){
            PyErr_Format(
                PyExc_ValueError,
                "invalid value for piece. piece must be in range from NCH_NO_PIECE"
                " to NCH_PIECE_NB (execluding NCH_PIECE_NB). got %i",
                p
            );
            return NCH_NO_PIECE;    
        }
    }
    else if(PyUnicode_Check(obj)){
        Py_ssize_t str_len = PyUnicode_GetLength(obj);
        if (str_len > 1){
            goto str_err;
        }

        const char* str = PyUnicode_AsUTF8(obj);
        const char c = *str;
        switch (c)
        {
        case 'P': p = NCH_WPawn  ; break;
        case 'N': p = NCH_WKnight; break;
        case 'B': p = NCH_WBishop; break;
        case 'R': p = NCH_WRook  ; break;
        case 'Q': p = NCH_WQueen ; break;
        case 'K': p = NCH_WKing  ; break;
        case 'p': p = NCH_BPawn  ; break;
        case 'n': p = NCH_BKnight; break;
        case 'b': p = NCH_BBishop; break;
        case 'r': p = NCH_BRook  ; break;
        case 'q': p = NCH_BQueen ; break;
        case 'k': p = NCH_BKing  ; break;
        default:
            goto str_err;
        }
    }
    else{
        PyErr_Format(
            PyExc_ValueError,
            "piece could only be int or str with one char. got %s",
            Py_TYPE(obj)->tp_name
        );
        return NCH_NO_PIECE;
    }

    return p;

    str_err:
        PyErr_SetString(
            PyExc_ValueError,
            "invalid string for piece. expected one char only from these \"PNBRQKpnbrqk\"."
        );
        return NCH_NO_PIECE;
}

int
pyobject_as_move(PyObject* obj, Move* dst_move){
    if (PyUnicode_Check(obj)) {
        const char* move_str = PyUnicode_AsUTF8(obj);
        if (PyErr_Occurred()){
            return 0;
        }

        if (!Move_FromString(move_str, dst_move)){
            PyErr_SetString(PyExc_ValueError, "invalid string to create a move");
            return 0;
        }
    
    } 
    else if (PyMove_Check(obj) || PyLong_Check(obj)){
        *dst_move = PyMove_AsMove(obj);
        if (PyErr_Occurred()){
            return 0;
        }
    } 
    else {
        PyErr_Format(
            PyExc_TypeError,
            "step must be a Move object, string or int, got %s",
            Py_TYPE(obj)->tp_name
        );
        return 0;
    }

    return 1;
}

MoveType
pyobject_as_move_type(PyObject* obj){
    if (Py_IsNone(obj))
        return MoveType_Normal;

    MoveType type;
    if (PyLong_Check(obj)){
        type = (MoveType)PyLong_AsLongLong(obj);
        if (!MoveType_IsValid(type)){
            PyErr_Format(
                PyExc_ValueError,
                "invalid value for move type. move type must be in range from 0 to 3"
                "(MoveType_Normal to MoveType_Castle). got %i",
                type
            );
            return MoveType_Null;    
        }
    }
    else if (PyUnicode_Check(obj)){
        const char* str = PyUnicode_AsUTF8(obj);
        if (!str) {
            PyErr_SetString(PyExc_ValueError, "failed to read the string");
            return MoveType_Null;
        }

        for (type = MoveType_Normal; type < MoveType_NB; type++){
            if (strcmp(str, MoveType2Str[type]) == 0)
                return type;
        }

        PyErr_Format(
            PyExc_ValueError,
            "invalid string for move type. expected one of these (%s, %s, %s, %s) got %s",
            MoveType2Str[MoveType_Normal],
            MoveType2Str[MoveType_Promotion],
            MoveType2Str[MoveType_EnPassant],
            MoveType2Str[MoveType_Castle],
            str
        );

        return MoveType_Null;
    }
    else{
        PyErr_Format(
            PyExc_ValueError,
            "move type expeted to be int or string of these (%s, %s, %s, %s) got %s",
            MoveType2Str[MoveType_Normal],
            MoveType2Str[MoveType_Promotion],
            MoveType2Str[MoveType_EnPassant],
            MoveType2Str[MoveType_Castle],
            Py_TYPE(obj)->tp_name
        );

        return MoveType_Null; 
    }
    return type;
}

Side
pyobject_as_side(PyObject* obj, int support_both_sides){
    if (Py_IsNone(obj)){
        return NCH_NO_SIDE;
    }
    if (PyLong_Check(obj)){
        Side side = PyLong_AsLongLong(obj);
        if (PyErr_Occurred())
            return NCH_NO_SIDE;
        
        if (!is_valid_side(side) && !(support_both_sides && side == NCH_SIDES_NB)){
            PyErr_Format(
                PyExc_ValueError,
                "side must be 0 or 1 (white or black). got %d",
                side
            );
            return NCH_NO_SIDE;
        }

        return side;
    }
    else{
        PyErr_Format(
            PyExc_ValueError,
            "side must be an int. 0 for white, 1 for black. got ",
            Py_TYPE(obj)->tp_name
        );
        return NCH_NO_SIDE;
    }
}
