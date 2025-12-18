#include "square_functions.h"
#include "common.h"
#include <math.h>

PyObject*
square_from_uci(PyObject* self, PyObject* args){
    char* s_str;
    if (!PyArg_ParseTuple(args, "s", &s_str)){
        if (!PyErr_Occurred()){
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
square_column(PyObject* self, PyObject* args){
    PyObject* s;
    if (!PyArg_ParseTuple(args, "O", &s)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the arguments to get the file of a square");
        return NULL;
    }
    
    Square sqr = pyobject_as_square(s);
    if (sqr == NCH_NO_SQR){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "cant not return the column of a NO_SQUARE");
        }
        return NULL;
    }

    int column = NCH_GET_COLIDX(sqr);
    return PyLong_FromLong(column);
}

PyObject*
square_row(PyObject* self, PyObject* args){
    PyObject* s;
    if (!PyArg_ParseTuple(args, "O", &s)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the arguments to get the rank of a square");
        return NULL;
    }

    Square sqr = pyobject_as_square(s);
    if (sqr == NCH_NO_SQR){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "cant not return the row of a NO_SQUARE");
        }
        return NULL;
    }

    int row = NCH_GET_ROWIDX(sqr);
    return PyLong_FromLong(row);
}

PyObject*
square_distance(PyObject* self, PyObject* args){
    PyObject* s1, *s2;
    if (!PyArg_ParseTuple(args, "OO", &s1, &s2)){
        PyErr_SetString(
            PyExc_ValueError,
            "failed to parse the arguments to calculate the distance between two squares"
        );
        return NULL;
    }

    Square sqr1 = pyobject_as_square(s1);
    CHECK_NO_SQUARE_ERR(sqr1, NULL)

    Square sqr2 = pyobject_as_square(s2);
    CHECK_NO_SQUARE_ERR(sqr1, NULL)

    int dr = abs(NCH_GET_ROWIDX(sqr1) - NCH_GET_ROWIDX(sqr2));
    int dc = abs(NCH_GET_COLIDX(sqr1) - NCH_GET_COLIDX(sqr2));
    int distance = dr > dc ? dr : dc;
    return PyLong_FromLong(distance);
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
    if (sqr == NCH_NO_SQR){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "can't return the mirror of a NO_SQUARE");
        }
        return NULL;
    }

    Square mirror = is_vertical ? NCH_SQR_MIRROR_V(sqr)
                                : NCH_SQR_MIRROR_H(sqr);

    return square_to_pyobject(mirror);
}

