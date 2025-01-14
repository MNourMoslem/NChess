#include "pyboard.h"
#include "src/nchess.h"
#include <numpy/arrayobject.h>

PyObject*
move_as_uci(PyObject* self, PyObject* args){
    PyObject* m;
    if (!PyArg_ParseTuple(args, "O", &m)){
        PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        return NULL;
    }

    if (!PyLong_Check(m)){
        PyErr_Format(PyExc_ValueError,
         "move expcted to be an int. got %s",
         Py_TYPE(m)->tp_name);
    }

    Move move = PyLong_AsUnsignedLong(m);
    char buffer[10];

    int res = Move_AsString(move, buffer);
    if (res == -1)
        Py_RETURN_NONE;

    return PyUnicode_FromString(buffer);
}

// Method definitions
static PyMethodDef nchess_methods[] = {
    {"move_as_uci", (PyCFunction)move_as_uci, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}  // Sentinel
};

static PyModuleDef nchess = {
    PyModuleDef_HEAD_INIT,
    .m_name = "nchess",
    .m_doc = "NChess module",
    .m_size = -1,
    .m_methods = &nchess_methods
};

// Initialize the module
PyMODINIT_FUNC PyInit_nchess(void) {
    PyObject* m;
    if (PyType_Ready(&PyBoardType) < 0) return NULL;

    m = PyModule_Create(&nchess);
    if (m == NULL) return NULL;

    Py_INCREF(&PyBoardType);
    PyModule_AddObject(m, "Board", (PyObject*)&PyBoardType);

    NCH_Init();
    import_array();

    return m;
}
