#include "pyboard_getset.h"
#include "nchess/nchess.h"
#include "pyboard.h"
#include "pybb.h"

#define BOARD(obj) ((PyBoard*)obj)->board

PyObject*
board_get_white_pawns(PyObject* self, void* something){
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(Board_WHITE_PAWNS(BOARD(self)));
}

PyObject*
board_get_black_pawns(PyObject* self, void* something){
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(Board_BLACK_PAWNS(BOARD(self)));
}

PyObject*
board_get_white_knights(PyObject* self, void* something){
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(Board_WHITE_KNIGHTS(BOARD(self)));
}

PyObject*
board_get_black_knights(PyObject* self, void* something){
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(Board_BLACK_KNIGHTS(BOARD(self)));
}

PyObject*
board_get_white_bishops(PyObject* self, void* something){
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(Board_WHITE_BISHOPS(BOARD(self)));
}

PyObject*
board_get_black_bishops(PyObject* self, void* something){
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(Board_BLACK_BISHOPS(BOARD(self)));
}

PyObject*
board_get_white_rooks(PyObject* self, void* something){
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(Board_WHITE_ROOKS(BOARD(self)));
}

PyObject*
board_get_black_rooks(PyObject* self, void* something){
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(Board_BLACK_ROOKS(BOARD(self)));
}

PyObject*
board_get_white_queens(PyObject* self, void* something){
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(Board_WHITE_QUEENS(BOARD(self)));
}

PyObject*
board_get_black_queens(PyObject* self, void* something){
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(Board_BLACK_QUEENS(BOARD(self)));
}

PyObject*
board_get_white_king(PyObject* self, void* something){
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(Board_WHITE_KING(BOARD(self)));
}

PyObject*
board_get_black_king(PyObject* self, void* something){
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(Board_BLACK_KING(BOARD(self)));
}

PyObject*
board_get_white_occ(PyObject* self, void* something){
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(Board_WHITE_OCC(BOARD(self)));
}

PyObject*
board_get_black_occ(PyObject* self, void* something){
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(Board_BLACK_OCC(BOARD(self)));
}

PyObject*
board_get_all_occ(PyObject* self, void* something){
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(Board_ALL_OCC(BOARD(self)));
}

PyObject*
board_castles(PyObject* self, void* something){
    return (PyObject*)PyBitBoard_FromUnsignedLongLong(Board_CASTLES(BOARD(self)));
}

PyObject*
board_castles_str(PyObject* self, void* something){
    uint8 castles = Board_CASTLES(BOARD(self));
    if (!castles)
        Py_RETURN_NONE;

    char buffer[5];
    int i = 0;

    if (Board_IS_CASTLE_WK(BOARD(self)))
        buffer[i++] = 'K';
    if (Board_IS_CASTLE_WQ(BOARD(self)))
        buffer[i++] = 'Q';
    if (Board_IS_CASTLE_BK(BOARD(self)))
        buffer[i++] = 'k';
    if (Board_IS_CASTLE_BQ(BOARD(self)))
        buffer[i++] = 'q';

    if (!i)
        buffer[i++] = '-';

    buffer[i] = '\0';

    return PyUnicode_FromString(buffer);
}

PyObject*
board_nmoves(PyObject* self, void* something){
    return PyLong_FromLong(Board_NMOVES(BOARD(self)));
}

PyObject*
board_fifty_counter(PyObject* self, void* something){
    return PyLong_FromLong(Board_FIFTY_COUNTER(BOARD(self)));
}

PyObject*
board_en_passant_square(PyObject* self, void* something){
    Board* b = BOARD(self);
    if (!Board_ENP_IDX(b))
        Py_RETURN_NONE;

    return PyLong_FromLong(NCH_SQRIDX(Board_ENP_TRG(b)));
}

PyGetSetDef pyboard_getset[] = {
    {"white_pawns"   , (getter)board_get_white_pawns  , NULL, NULL, NULL},
    {"black_pawns"   , (getter)board_get_black_pawns  , NULL, NULL, NULL},
    {"white_knights" , (getter)board_get_white_knights, NULL, NULL, NULL},
    {"black_knights" , (getter)board_get_black_knights, NULL, NULL, NULL},
    {"white_bishops" , (getter)board_get_white_bishops, NULL, NULL, NULL},
    {"black_bishops" , (getter)board_get_black_bishops, NULL, NULL, NULL},
    {"white_rooks"   , (getter)board_get_white_rooks  , NULL, NULL, NULL},
    {"black_rooks"   , (getter)board_get_black_rooks  , NULL, NULL, NULL},
    {"white_queens"  , (getter)board_get_white_queens , NULL, NULL, NULL},
    {"black_queens"  , (getter)board_get_black_queens , NULL, NULL, NULL},
    {"white_king"    , (getter)board_get_white_king   , NULL, NULL, NULL},
    {"black_king"    , (getter)board_get_black_king   , NULL, NULL, NULL},
    {"white_occ"     , (getter)board_get_white_occ    , NULL, NULL, NULL},
    {"black_occ"     , (getter)board_get_black_occ    , NULL, NULL, NULL},
    {"all_occ"       , (getter)board_get_all_occ      , NULL, NULL, NULL},
    {"castles"       , (getter)board_castles          , NULL, NULL, NULL},
    {"castles_str"   , (getter)board_castles_str      , NULL, NULL, NULL},
    {"nmoves"        , (getter)board_nmoves           , NULL, NULL, NULL},
    {"fifty_counter" , (getter)board_fifty_counter    , NULL, NULL, NULL},
    {"en_passant_sqr", (getter)board_en_passant_square, NULL, NULL, NULL},
    {NULL            , NULL                           , NULL, NULL, NULL},
};