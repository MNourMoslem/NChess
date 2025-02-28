#include "pymove.h"
#include "common.h"
#include "nchess/nchess.h"


PyMove*
PyMove_FromMove(Move move){
    PyObject* obj = PyLong_FromUnsignedLong(move);
    if (!obj){
        PyErr_SetString(
            PyExc_ValueError,
            "Falied to create a move object"
        );
        return NULL;
    }
    obj->ob_type = &PyMoveType;
    return obj;
}

PyObject*
PyMove_FromUCI(PyObject* self, PyObject* args, PyObject* kwargs){
    const char* uci;
    PyObject* move_type = NULL;
    static char* kwlist[] = {"uci", "move_type", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|O", kwlist, &uci, &move_type)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    Move move = pyobject_as_move(uci);                        
    if (PyErr_Occurred())
        return NULL;
    
    if (move_type){
        MoveType mt = pyobject_as_move_type(move_type);
        if (mt == MoveType_Null)
            return NULL;

        move = Move_REASSAGIN_TYPE(move, mt);
    }

    return (PyObject*)PyMove_FromMove(move);
}

PyObject*
PyMove_FromArgs(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject *from_, *to_;
    PyObject * promote = NULL;
    PyObject * type = NULL;
    static char* kwlist[] = {"from_", "to_", "promote", "type", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO|OO", kwlist, &from_, &to_, &promote, &type)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    Square f = pyobject_as_square(from_);
    if (f == NCH_NO_SQR){
        if (!PyErr_Occurred()){
            PyErr_SetString(
                PyExc_ValueError,
                "from_ square must be a valid square from 0 to 63 and can't be None"
            );
        }
        return NULL;
    }

    Square t = pyobject_as_square(to_);
    if (t == NCH_NO_SQR){
        if (!PyErr_Occurred()){
            PyErr_SetString(
                PyExc_ValueError,
                "to_ square must be a valid square from 0 to 63 and can't be None"
            );
        }
        return NULL;
    }

    PieceType pt;
    if (promote){
        pt = pyobject_as_piece_type(promote);
        if (PyErr_Occurred())
            return NULL;
    }
    else{
        pt = NCH_NO_PIECE_TYPE;
    }

    MoveType mt;
    if (type){
        mt = pyobject_as_move_type(type);
        if (mt == MoveType_Null)
            return NULL;
    }
    else{
        mt = MoveType_Normal;
    }

    Move move = Move_New(f, t, pt, mt);
    if (move == Move_NULL){
        PyErr_SetString(PyExc_ValueError, "invalid move");
        return NULL;
    }

    return (PyObject*)PyMove_FromMove(move);
}

PyObject*
PyMove_Str(PyObject* self){
    char buffer[10];
    Move move = (Move)PyLong_AsUnsignedLong(self);
    int res = Move_AsString(move, buffer);
    if (res < 0){
        strcpy(buffer, "null");
    }
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


PyGetSetDef getset_methods[] = {
    {"from_"       , (getter)get_from_sqr     , NULL, NULL, NULL},
    {"to_"         , (getter)get_to_sqr       , NULL, NULL, NULL},
    {"pro_piece"   , (getter)get_pro_piece    , NULL, NULL, NULL},
    {"move_type"   , (getter)get_move_type    , NULL, NULL, NULL},
    {"is_normal"   , (getter)get_is_normal    , NULL, NULL, NULL},
    {"is_enpassant", (getter)get_is_enpassant , NULL, NULL, NULL},
    {"is_castle"   , (getter)get_is_castle    , NULL, NULL, NULL},
    {"is_promotion", (getter)get_is_promotion , NULL, NULL, NULL},
    {"is_valid"    , (getter)get_is_valid     , NULL, NULL, NULL},
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