#ifndef NCHESS_CORE_BB_FUNCTIONS_H
#define NCHESS_CORE_BB_FUNCTIONS_H

#include "src/nchess.h"
#define PY_SSIZE_CLEAN_T
#include <Python.h>


NCH_STATIC_INLINE void
bb2array(uint64 bb, int* arr, int reverse){
    // memset(arr, 0, sizeof(sizeof(int) * NCH_SQUARE_NB));
    for (int i = 0; i < NCH_SQUARE_NB; i++){
        arr[i] = 0;
    }

    int idx;
    if (reverse){
        LOOP_U64_T(bb){
            arr[63 - idx] = 1;
        }
    }
    else{
        LOOP_U64_T(bb){
            arr[idx] = 1;
        }
    }
}

PyObject* BB_AsArray(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* BB_MoreThenOne(PyObject* self, PyObject* args);
PyObject* BB_HasTwoBits(PyObject* self, PyObject* args);
PyObject* BB_GetTSB(PyObject* self, PyObject* args);
PyObject* BB_GetLSB(PyObject* self, PyObject* args);
PyObject* BB_CountBits(PyObject* self, PyObject* args);
PyObject* BB_IsFilled(PyObject* self, PyObject* args, PyObject* kwargs);

#endif // NCHESS_CORE_BB_FUNCTIONS_H