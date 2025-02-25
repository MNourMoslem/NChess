#include "square_functions.h"
#include "common.h"

PyObject*
uci_as_square(PyObject* self, PyObject* args){
    PyObject* uni;
    if (!PyArg_ParseTuple(args, "O", &uni)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the arguments to get the square from uci");
        return NULL;
    }

    if (!PyUnicode_Check(uni)){
        PyErr_Format(PyExc_ValueError,
         "square expected to be a string. got %s",
         Py_TYPE(uni)->tp_name);
        return NULL;
    }

    const char* s_str = PyUnicode_AsUTF8(uni);
    if (s_str == NULL) {
        PyErr_SetString(PyExc_ValueError, "failed to string to square");
        return NULL;
    }
    return square_to_pyobject(str_to_square(s_str));
}

PyObject*
square_file(PyObject* self, PyObject* args){
    PyObject* s;
    if (!PyArg_ParseTuple(args, "O", &s)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the arguments to get the file of a square");
        return NULL;
    }

    Square sqr = pyobject_as_square(s);
    if (!is_valid_square(sqr)){
        if (PyErr_Occurred())
            return NULL;

        Py_RETURN_NONE;
    }

    return PyLong_FromLong(NCH_GET_COLIDX(sqr));
}

PyObject*
square_rank(PyObject* self, PyObject* args){
    PyObject* s;
    if (!PyArg_ParseTuple(args, "O", &s)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the arguments to get the rank of a square");
        return NULL;
    }

    Square sqr = pyobject_as_square(s);
    if (!is_valid_square(sqr)){
        if (PyErr_Occurred())
            return NULL;

        Py_RETURN_NONE;
    }

    return PyLong_FromLong(NCH_GET_ROWIDX(sqr));
}

PyObject*
square_distance(PyObject* self, PyObject* args){
    PyObject* s1, *s2;
    if (!PyArg_ParseTuple(args, "OO", &s1, &s2)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the arguments to calculate the distance between two squares");
        return NULL;
    }

    Square sqr1 = pyobject_as_square(s1);
    Square sqr2 = pyobject_as_square(s2);
    if (!is_valid_square(sqr1) || !is_valid_square(sqr2)){
        if (PyErr_Occurred())
            return NULL;

        Py_RETURN_NONE;
    }

    return PyLong_FromLong(NCH_SQR_DISTANCE(sqr1, sqr2));
}

PyObject*
square_mirror(PyObject* self, PyObject* args){
    PyObject* s;
    int is_verical = 1;
    if (!PyArg_ParseTuple(args, "O|p", &s)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the arguments to mirror a square");
        return NULL;
    }

    Square sqr = pyobject_as_square(s);
    if (!is_valid_square(sqr)){
        if (PyErr_Occurred())
            return NULL;

        Py_RETURN_NONE;
    }

    return square_to_pyobject(is_verical ? NCH_SQR_MIRROR_V(sqr) : NCH_SQR_MIRROR_V(sqr));
}
