#include "common.h"
#include "pyboard.h"
#include "pymove.h"
#include "bb_functions.h"
#include "PyBB.h"
#include "array_conversion.h"
#include "square_functions.h"

#include "nchess/nchess.h"

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>
#include <stdio.h>

// Method definitions
static PyMethodDef nchess_core_methods[] = {
    {"square_from_uci"   , (PyCFunction)square_from_uci  , METH_VARARGS                , NULL},
    {"square_column"     , (PyCFunction)square_column    , METH_VARARGS                , NULL},
    {"square_row"        , (PyCFunction)square_row       , METH_VARARGS                , NULL},
    {"square_distance"   , (PyCFunction)square_distance  , METH_VARARGS                , NULL},
    {"square_mirror"     , (PyCFunction)square_mirror    , METH_VARARGS | METH_KEYWORDS, NULL},

    {"move"              , (PyCFunction)PyMove_FromArgs  , METH_VARARGS | METH_KEYWORDS, NULL},
    {"move_from_uci"     , (PyCFunction)PyMove_FromUCI   , METH_VARARGS | METH_KEYWORDS, NULL},

    {"bb_from_array"     , (PyCFunction)BB_FromArray     , METH_VARARGS | METH_KEYWORDS, NULL},
    {"bb_from_squares"   , (PyCFunction)BB_FromSquares   , METH_VARARGS | METH_KEYWORDS, NULL},
    {"bb_rook_attacks"   , (PyCFunction)BB_RookAttacks   , METH_VARARGS | METH_KEYWORDS, NULL},
    {"bb_bishop_attacks" , (PyCFunction)BB_BishopAttacks , METH_VARARGS | METH_KEYWORDS, NULL},
    {"bb_queen_attacks"  , (PyCFunction)BB_QueenAttacks  , METH_VARARGS | METH_KEYWORDS, NULL},
    {"bb_king_attacks"   , (PyCFunction)BB_KingAttacks   , METH_VARARGS | METH_KEYWORDS, NULL},
    {"bb_knight_attacks" , (PyCFunction)BB_KnightAttacks , METH_VARARGS | METH_KEYWORDS, NULL},
    {"bb_pawn_attacks"   , (PyCFunction)BB_PawnAttacks   , METH_VARARGS | METH_KEYWORDS, NULL},
    {"bb_rook_mask"      , (PyCFunction)BB_RookMask      , METH_VARARGS | METH_KEYWORDS, NULL},
    {"bb_bishop_mask"    , (PyCFunction)BB_BishopMask    , METH_VARARGS | METH_KEYWORDS, NULL},
    {"bb_rook_relevant"  , (PyCFunction)BB_RookRelevant  , METH_VARARGS | METH_KEYWORDS, NULL},
    {"bb_bishop_relevant", (PyCFunction)BB_BishopRelevant, METH_VARARGS | METH_KEYWORDS, NULL},
    {"bb_rook_magic"     , (PyCFunction)BB_RookMagic     , METH_VARARGS | METH_KEYWORDS, NULL},
    {"bb_bishop_magic"   , (PyCFunction)BB_BishopMagic   , METH_VARARGS | METH_KEYWORDS, NULL},
    {NULL                , NULL                          , 0                           , NULL},
};

static PyModuleDef nchess_core = {
    PyModuleDef_HEAD_INIT,
    .m_name = "nchess_core",
    .m_doc = "core module of nchess library",
    .m_size = -1,
    .m_methods = nchess_core_methods
};

// Initialize the module
PyMODINIT_FUNC PyInit_nchess_core(void) {
    PyObject* m;
    PyBitBoardType.tp_base = &PyLong_Type;
    PyMoveType.tp_base = &PyLong_Type;
    
    if (PyType_Ready(&PyBoardType) < 0) {
        return NULL;
    }
    
    if (PyType_Ready(&PyMoveType) < 0) {
        return NULL;
    }

    if (PyType_Ready(&PyBitBoardType) < 0) {
        return NULL;
    }

    // Create the module
    m = PyModule_Create(&nchess_core);
    if (m == NULL) {
        return NULL;
    }
    
    // Add PyBoardType to the module
    Py_INCREF(&PyBoardType);
    if (PyModule_AddObject(m, "Board", (PyObject*)&PyBoardType) < 0) {
        Py_DECREF(&PyBoardType);
        Py_DECREF(m);
        return NULL;
    }
    
    // Add PyMoveType to the module
    Py_INCREF(&PyMoveType);
    if (PyModule_AddObject(m, "Move", (PyObject*)&PyMoveType) < 0) {
        Py_DECREF(&PyMoveType);
        Py_DECREF(&PyBoardType);
        Py_DECREF(m);
        return NULL;
    }
    
    Py_INCREF(&PyBitBoardType);
    if (PyModule_AddObject(m, "BitBoard", (PyObject*)&PyBitBoardType) < 0) {
        Py_DECREF(&PyBitBoardType);
        Py_DECREF(&PyMoveType);
        Py_DECREF(&PyBoardType);
        Py_DECREF(m);
        return NULL;
    }
    
    // Initialize additional components
    NCH_Init();

    return m;
}

