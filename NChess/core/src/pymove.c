#include "pymove.h"
#include "common.h"
#include "nchess/nchess.h"

#define M(obj) ((PyMove*)obj)

PyMove*
PyMove_New(Move move){
    PyMove* pymove = PyObject_New(PyMove, &PyMoveType);
    if (pymove == NULL) {
        PyErr_NoMemory();
        return NULL;
    }

    pymove->move = move;

    return pymove;
}

PyObject*
new_move(PyTypeObject* type, PyObject* args, PyObject* kwds) {
    PyObject* from;
    PyObject* to;
    PyObject* promote = NULL;
    PyObject* castle = NULL;

    // Parse the arguments
    if (!PyArg_ParseTuple(args, "OO|OO", &from, &to, &promote, &castle)) {
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "Failed to parse the arguments to create a move");
        }
        return NULL;
    }

    Square from_sqr = pyobject_as_square(from);
    Square to_sqr = pyobject_as_square(to);
    Piece promote_piece = promote ? pyobject_as_piece(promote) : NCH_NO_PIECE;
    
    uint8 castle_type;
    if (castle){
        if (!PyLong_Check(castle)){
            PyErr_Format(PyExc_ValueError,
            "castle expected to be an int. got %s",
            Py_TYPE(castle)->tp_name);
            return NULL;
        }
        castle_type = (uint8)PyLong_AsUnsignedLong(castle);
    }
    else{
        castle_type = 0;
    }

    Move move;
    if (!is_valid_square(from_sqr) || !is_valid_square(to_sqr)){
        if (PyErr_Occurred())
            return NULL;
        move = 0;
    }
    else{
        move = Move_New(from_sqr, to_sqr, promote_piece, castle_type, 0, 0);
    }

    return PyMove_New(move);
}

PyObject*
PyMove_Str(PyObject* self){
    char buffer[10];
    Move_AsString(M(self)->move, buffer);
    return PyUnicode_FromFormat("%s(\"%s\")", Py_TYPE(self)->tp_name, buffer);
}

PyObject*
PyMove_AsInt(PyObject* self){
    return PyLong_FromUnsignedLong(M(self)->move);
}

PyNumberMethods number_methods = {
    .nb_int = (unaryfunc)PyMove_AsInt,
};

PyObject*
get_from_sqr(PyObject* self, void* something){
    return PyLong_FromUnsignedLong(Move_FROM(M(self)->move));
}

PyObject*
get_to_sqr(PyObject* self, void* something){
    return PyLong_FromUnsignedLong(Move_TO(M(self)->move));
}


PyObject*
get_promote_piece(PyObject* self, void* something){
    return PyLong_FromUnsignedLong(Move_PRO_PIECE(M(self)->move));
}

PyObject*
get_castle_type(PyObject* self, void* something){
    return PyLong_FromUnsignedLong(Move_CASTLE(M(self)->move));
}

PyObject*
get_is_enp(PyObject* self, void* something){
    return PyBool_FromLong(Move_IS_ENP(M(self)->move));
}

PyObject*
get_is_pro(PyObject* self, void* something){
    return PyBool_FromLong(Move_IS_PRO(M(self)->move));
}

PyGetSetDef getset_methods[] = {
    {"from_", (getter)get_from_sqr, NULL, NULL, NULL},
    {"to_", (getter)get_to_sqr, NULL, NULL, NULL},
    {"promote", (getter)get_promote_piece, NULL, NULL, NULL},
    {"castle", (getter)get_castle_type, NULL, NULL, NULL},
    {"is_enpassant", (getter)get_is_enp, NULL, NULL, NULL},
    {"is_promotion", (getter)get_is_pro, NULL, NULL, NULL},
};

PyTypeObject PyMoveType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Move",
    .tp_doc = "Move object",
    .tp_basicsize = sizeof(PyMove),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = new_move,
    .tp_str = PyMove_Str,
    .tp_as_number = &number_methods,
    .tp_getset = &getset_methods,
    .tp_repr = PyMove_Str,
};