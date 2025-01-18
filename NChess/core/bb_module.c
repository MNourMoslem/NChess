#include "bb_module.h"
#include "bb_functions.h"

static PyMethodDef bb_methods[] = {
    {"as_array", (PyCFunction)BB_AsArray, METH_VARARGS | METH_KEYWORDS, NULL},
    {"more_then_one", (PyCFunction)BB_MoreThenOne, METH_VARARGS, NULL},
    {"has_two_bits", (PyCFunction)BB_HasTwoBits, METH_VARARGS, NULL},
    {"get_tsb", (PyCFunction)BB_GetTSB, METH_VARARGS, NULL},
    {"get_lsb", (PyCFunction)BB_GetLSB, METH_VARARGS, NULL},
    {"is_filled", (PyCFunction)BB_IsFilled, METH_VARARGS | METH_KEYWORDS, NULL},
    {"from_array", (PyCFunction)BB_FromArray, METH_VARARGS | METH_KEYWORDS, NULL},
    {"rook_attacks", (PyCFunction)BB_RookAttacks, METH_VARARGS | METH_KEYWORDS, NULL},
    {"bishop_attacks", (PyCFunction)BB_BishopAttacks, METH_VARARGS | METH_KEYWORDS, NULL},
    {"queen_attacks", (PyCFunction)BB_QueenAttacks, METH_VARARGS | METH_KEYWORDS, NULL},
    {"king_attacks", (PyCFunction)BB_KingAttacks, METH_VARARGS | METH_KEYWORDS, NULL},
    {"knight_attacks", (PyCFunction)BB_KnightAttacks, METH_VARARGS | METH_KEYWORDS, NULL},
    {"pawn_attacks", (PyCFunction)BB_PawnAttacks, METH_VARARGS | METH_KEYWORDS, NULL},
    {"rook_mask", (PyCFunction)BB_RookMask, METH_VARARGS | METH_KEYWORDS, NULL},
    {"bishop_mask", (PyCFunction)BB_BishopMask, METH_VARARGS | METH_KEYWORDS, NULL},
    {"rook_relevant", (PyCFunction)BB_RookRelevant, METH_VARARGS | METH_KEYWORDS, NULL},
    {"bishop_relevant", (PyCFunction)BB_BishopRelevant, METH_VARARGS | METH_KEYWORDS, NULL},
    {"rook_magic", (PyCFunction)BB_RookMagic, METH_VARARGS | METH_KEYWORDS, NULL},
    {"bishop_magic", (PyCFunction)BB_BishopMagic, METH_VARARGS | METH_KEYWORDS, NULL},
    {"to_indeices", (PyCFunction)BB_ToIndeices, METH_VARARGS | METH_KEYWORDS, NULL},
    {NULL, NULL, 0, NULL}  // Sentinel
};

PyModuleDef bb_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "nchess.bb",
    .m_doc = "BitBoard Moudle of nchess",
    .m_size = -1,
    .m_methods = &bb_methods
};

PyMODINIT_FUNC PyInit_bb_module(void) {
    return PyModule_Create(&bb_module);
}
