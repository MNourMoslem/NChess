#include "pymove.h"
#include "common.h"
#include "nchess/nchess.h"
#include "pymove_getset.h"

PyMove*
PyMove_FromMove(Move move)
{
    PyObject* args = Py_BuildValue("(k)", (unsigned long)move);
    if (!args) {
        return NULL;
    }
    
    PyObject* obj = PyLong_Type.tp_new(&PyMoveType, args, NULL);
    Py_DECREF(args);  // tp_new doesn't steal the reference
    
    if (!obj) {
        return NULL;
    }

    return (PyMove*)obj;
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

    Move move;
    if (!Move_FromString(uci, &move)){
        PyErr_SetString(
            PyExc_ValueError,
            "uci string was invalid to create a move"
        );
        return NULL;
    }

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
    CHECK_NO_SQUARE_ERR(f, NULL);

    Square t = pyobject_as_square(to_);
    CHECK_NO_SQUARE_ERR(t, NULL);

    PieceType pt;
    if (promote){
        pt = pyobject_as_piece_type(promote);
        if (pt == NCH_NO_PIECE_TYPE)
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
    return (PyObject*)PyMove_FromMove(move);
}

PyObject*
PyMove_Str(PyObject* self){
    Move move = (Move)PyMove_AsMove(self);
    if (PyErr_Occurred()){
        return NULL;
    }
    
    char buffer[10];
    int res = Move_AsString(move, buffer);
    if (res < 0){
        strcpy(buffer, "null");
    }
    return PyUnicode_FromFormat("%s(\"%s\")", Py_TYPE(self)->tp_name, buffer);
}

PyObject*
move_new(PyTypeObject* type, PyObject* args, PyObject* kwargs){
    PyObject* obj;
    static char* kwlist[] = {"move", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &obj)) {
        return NULL;
    }

    Move move;
    if (!pyobject_as_move(obj, &move)) {
        return NULL;
    }

    return (PyObject*)PyMove_FromMove(move);
}


PyTypeObject PyMoveType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "Move",
    .tp_doc = "Move object",
    .tp_basicsize = sizeof(PyMove),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_str = PyMove_Str,
    .tp_repr = PyMove_Str,
    .tp_getset = pymove_getset,
    .tp_new = move_new,
};