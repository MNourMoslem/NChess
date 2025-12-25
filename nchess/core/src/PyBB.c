#include "bb_functions.h"
#include "PyBB.h"
#include "array_conversion.h"
#include "common.h"
#include "pybb_methods.h"

#define PY_SSIZE_CLEAN_T
#include <Python.h>

PyObject*
pybitboard_new(PyTypeObject* type, PyObject* args, PyObject* kwargs){
    unsigned long long value;    
    if (!PyArg_ParseTuple(args, "K", &value)) {
        return NULL;
    }

    return PyLong_Type.tp_new(type, args, kwargs);
}

PyBitBoard* PyBitBoard_FromArrayLike(PyObject* array_like){
    uint64 bb = bb_from_object(array_like);
    if (PyErr_Occurred()){
        return NULL;
    }

    return PyBitBoard_FromUnsignedLongLong(bb);
}

PyBitBoard* PyBitBoard_FromUnsignedLongLong(unsigned long long value){
    PyObject* args = Py_BuildValue("(K)", value);
    if (!args) {
        return NULL;
    }
    
    PyBitBoard* result = pybitboard_new(&PyBitBoardType, args, NULL);
    Py_DECREF(args);  // tp_new doesn't steal the reference
    return result;
}

NCH_STATIC PyObject*
bb_new(PyTypeObject* type, PyObject* args, PyObject* kwargs) {
    PyObject* obj = NULL;
    if (!PyArg_ParseTuple(args, "|O", &obj)) {
        return NULL;
    }

    if (!obj){
        return PyBitBoard_FromUnsignedLongLong(0); 
    }

    if (PyLong_Check(obj)) {
        unsigned long long value = PyLong_AsUnsignedLongLong(obj);
        if (PyErr_Occurred()) {
            return NULL;
        }
        return PyBitBoard_FromUnsignedLongLong(value);
    }
    else if (PySequence_Check(obj)) {
        return (PyObject*)PyBitBoard_FromArrayLike(obj);
    }
    else {
        PyErr_Format(
            PyExc_TypeError,
            "BitBoard constructor expects an int, got %s",
            Py_TYPE(obj)->tp_name
        );
        return NULL;
    }
}

NCH_STATIC PyObject*
bb_iter(PyObject* self){
    // just create a tuple and iterate over it
    uint64 bb = BB_FromLong(self);
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
        PyObject* sqr_obj = square_to_pyobject(idx);
        if (!sqr_obj) {
            Py_DECREF(tuple);
            return NULL;
        }
        PyTuple_SetItem(tuple, i++, sqr_obj);
    }

    // iterate the tuple
    PyObject* iter = PyObject_GetIter(tuple);
    Py_DECREF(tuple);  // Iterator takes its own reference
    return iter;
}

static PyObject*
bb_str(PyBitBoard* self) {
    unsigned long long value = BB_FromLong((PyObject*)self);
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