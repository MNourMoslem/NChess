#ifndef NCHESS_CORE_BB_MODULE_H
#define NCHESS_CORE_BB_MODULE_H

#include "nchess/nchess.h"
#define PY_SSIZE_CLEAN_T
#include <Python.h>

extern PyModuleDef bb_module;

PyMODINIT_FUNC PyInit_bb_module(void);

#endif