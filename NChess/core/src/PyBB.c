#include "bb_functions.h"
#include "pybb.h"
#include "array_conversion.h"
#include "common.h"
#include "pybb_methods.h"

#define PY_SSIZE_CLEAN_T
#include <Python.h>

PyBitBoard* PyBitBoard_FromUnsignedLongLong(unsigned long long value){
    PyObject* obj = PyLong_FromUnsignedLongLong(value);
    if (!obj){
        if (!PyErr_Occurred()){
            PyErr_SetString(
                PyExc_ValueError,
                "Falied to create a bitboard object"
            );
        }
        return NULL;
    }
    obj->ob_type = &PyBitBoardType;
    return (PyBitBoard*)obj;
}

NCH_STATIC PyObject*
bb_new(PyTypeObject* type, PyObject* args, PyObject* kwargs) {
    unsigned long long value;    
    if (!PyArg_ParseTuple(args, "L", &value)) {
        return NULL;
    }

    return PyLong_Type.tp_new(type, args, kwargs);
}

NCH_STATIC PyObject*
bb_iter(PyObject* self){
    // just create a tuple and iterate over it
    uint64 bb = PyLong_AsUnsignedLongLong(self);
    if (PyErr_Occurred()) {
        return NULL;
    }
    PyObject* tuple = PyTuple_New(count_bits(bb));
    if (!tuple){
        return NULL;
    }

    Py_ssize_t i = 0;
    Square idx;
    LOOP_U64_T(bb){
        PyTuple_SetItem(tuple, i++, square_to_pyobject(idx));
    }

    // iterate the tuple
    return PyObject_GetIter(tuple);
}

static PyObject*
bb_str(PyBitBoard* self) {
    unsigned long long value = PyLong_AsUnsignedLongLong((PyObject*)self);
    if (PyErr_Occurred()) {
        return NULL;
    }

    PyObject* class_name_obj = PyObject_GetAttrString((PyObject*)Py_TYPE(self), "__name__");
    if (!class_name_obj) {
        return PyUnicode_FromString("<UnknownClass>");
    }

    // Use C snprintf() to format the hexadecimal representation
    char hex_buffer[20];  // Enough for "0x" + 16 hex digits + null terminator
    snprintf(hex_buffer, sizeof(hex_buffer), "0x%llx", value);

    // Create the final Python string
    PyObject* result = PyUnicode_FromFormat("%U(%s)", class_name_obj, hex_buffer);
    Py_DECREF(class_name_obj);
    
    return result;
}

PyTypeObject PyBitBoardType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "nchess_core.BitBoard",
    .tp_doc = "BitBoard object (inherits from int)",
    .tp_basicsize = sizeof(PyBitBoard),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_str = (reprfunc)bb_str,
    .tp_repr = (reprfunc)bb_str,
    .tp_methods = pybb_methods,
    .tp_iter = bb_iter,
    .tp_new = bb_new,
};