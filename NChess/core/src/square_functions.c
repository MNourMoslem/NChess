#include "square_functions.h"
#include "common.h"

PyObject*
square_from_uci(PyObject* self, PyObject* args){
    char* s_str;
    if (!PyArg_ParseTuple(args, "s", &s_str)){
        if (!PyErr_Occurred){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments to get the square from uci");
        }
        return NULL;
    }

    Square sqr = str_to_square(s_str);
    if (sqr == NCH_NO_SQR){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to convert string to square");
        }
        return NULL;
    }

    return square_to_pyobject(sqr);
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
square_mirror(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* s;
    int is_vertical = 1;
    NCH_STATIC char* kwlist[] = {"square", "vertical", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|p", kwlist, &s, &is_vertical)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the arguments to mirror a square");
        return NULL;
    }

    Square sqr = pyobject_as_square(s);
    if (!is_valid_square(sqr)){
        if (PyErr_Occurred())
            return NULL;

        Py_RETURN_NONE;
    }

    Square mirror = is_vertical ? NCH_SQR_MIRROR_V(sqr)
                                : NCH_SQR_MIRROR_H(sqr);

    return square_to_pyobject(mirror);
}

