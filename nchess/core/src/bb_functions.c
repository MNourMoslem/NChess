#include "common.h"
#include "array_conversion.h"
#include "bb_functions.h"
#include "PyBB.h"

#include "nchess/bit_operations.h"

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

void
bb2array(uint64 bb, int* arr, int reverse){
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

NCH_STATIC_INLINE int
parse_bb(uint64* bb, PyObject* args){
    if (!PyArg_ParseTuple(args, "K", bb)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return -1;
    }
    return 0;
}

NCH_STATIC int
discover_sequence_shape(PyObject* seq, npy_intp* dims, int dim){
    Py_ssize_t len = PySequence_Size(seq);
    if (len < 0)
        return -1;

    dims[dim] = len;

    if (!len)
        return dim;

    PyObject* first = PySequence_GetItem(seq, 0);
    if (!first){
        PyErr_SetString(PyExc_ValueError, "failed to getitem from the shape");
        return -1;
    }

    if (PySequence_Check(first)){
        int res = discover_sequence_shape(first, dims, dim+1);
        Py_DECREF(first);
        return res;
    }

    Py_DECREF(first);
    return dim;
}

NCH_STATIC int
seq2bb_internal(PyObject* seq, uint64* bb, Py_ssize_t* idx, npy_intp* dims, int depth, int max_depth){
    Py_ssize_t len = PySequence_Size(seq);
    if (len < 0){
        PyErr_SetString(PyExc_ValueError, "failed to get the length of the sequence object");
        return -1;
    }

    // Validate shape at current depth
    if (len != dims[depth]){
        PyErr_Format(PyExc_ValueError,
            "sequence shape mismatch at depth %d: expected %ld elements but got %ld",
            depth, (long)dims[depth], (long)len);
        return -1;
    }

    int res;
    PyObject* item;
    for (Py_ssize_t i = 0; i < len; i++){
        item = PySequence_GetItem(seq, i);
        if (!item){
            return -1;
        }

        if (PyLong_Check(item)){
            // Should be at the deepest level
            if (depth != max_depth){
                PyErr_Format(PyExc_ValueError,
                    "found integer at depth %d but expected sequence (max depth is %d)",
                    depth, max_depth);
                Py_DECREF(item);
                return -1;
            }
            
            if (*idx < 64){
                *bb |= PyLong_AsLong(item) ? NCH_SQR(*idx) : 0;
            }
            else{
                PyErr_SetString(PyExc_ValueError, "bitboard sequence should have 64 items, got more");
                Py_DECREF(item);
                return -1;
            }
            (*idx)++;
        }
        else if (PySequence_Check(item)){
            // Should not be at the deepest level
            if (depth == max_depth){
                PyErr_Format(PyExc_ValueError,
                    "found sequence at depth %d but expected integer (max depth is %d)",
                    depth, max_depth);
                Py_DECREF(item);
                return -1;
            }
            
            res = seq2bb_internal(item, bb, idx, dims, depth + 1, max_depth);
            if (res < 0){
                Py_DECREF(item);
                return -1;
            }
        }
        else{
            PyErr_Format(PyExc_ValueError,
            "bitboard sequence should contain int or sequence type objects (list, tuple, ...), got %s",
            Py_TYPE(item)->tp_name);
            
            Py_DECREF(item);
            return -1;
        }
        Py_DECREF(item);
    }

    return 0;
}

NCH_STATIC int
seq2bb(PyObject* seq, uint64* bb, Py_ssize_t* idx){
    // Discover the shape of the sequence
    npy_intp dims[NPY_MAXDIMS];
    int max_depth = discover_sequence_shape(seq, dims, 0);
    
    if (max_depth < 0){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to discover sequence shape");
        }
        return -1;
    }

    // Calculate total elements
    npy_intp total_elements = 1;
    for (int i = 0; i <= max_depth; i++){
        total_elements *= dims[i];
    }

    if (total_elements != 64){
        PyErr_Format(PyExc_ValueError,
            "bitboard sequence must contain exactly 64 elements, got %ld",
            (long)total_elements);
        return -1;
    }

    // Call internal function with shape validation
    return seq2bb_internal(seq, bb, idx, dims, 0, max_depth);
}

NCH_STATIC int
npy2bb(PyArrayObject* arr, uint64* bb) {
    if (!PyArray_Check(arr)) {
        PyErr_SetString(PyExc_TypeError, "Expected a NumPy array");
        return -1;
    }

    npy_intp num_elements = PyArray_SIZE(arr);
    if (num_elements != 64) {
        PyErr_Format(PyExc_ValueError, "Array must contain exactly 64 elements, but got %" NPY_INTP_FMT, num_elements);
        return -1;
    }

    if (!PyArray_ISINTEGER(arr)) {
        PyErr_SetString(PyExc_TypeError, "Array must contain integer elements");
        return -1;
    }

    PyArrayIterObject* iter = (PyArrayIterObject*)PyArray_IterNew((PyObject*)arr);
    if (!iter) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create array iterator");
        return -1;
    }

    *bb = 0;
    npy_intp idx = 0;

    while (PyArray_ITER_NOTDONE(iter)) {
        long value = *(long*)PyArray_ITER_DATA(iter);

        if (value != 0) {
            *bb |= (1ULL << idx);
        }

        idx++;
        PyArray_ITER_NEXT(iter);
    }

    Py_DECREF(iter);
    return 0;
}

uint64
bb_from_object(PyObject* obj) {
    uint64 bb = 0;

    if (PyLong_Check(obj)){
        return BB_FromLong(obj);
    }

    if (_import_array() < 0) {
        PyErr_SetString(PyExc_ImportError, "Failed to import NumPy");
        return 0;
    }
    
    if (PyArray_Check(obj)) {
        if (npy2bb((PyArrayObject*)obj, &bb) < 0) {
            return 0;
        }
    }
    else if (PySequence_Check(obj)) {
        Py_ssize_t idx = 0;
        if (seq2bb(obj, &bb, &idx) < 0) {
            return 0;
        }
    }
    else {
        PyErr_Format(PyExc_TypeError,
                     "Unsupported input type: expected int, NumPy array or sequence, got %s",
                     Py_TYPE(obj)->tp_name);
        return 0;
    }

    return bb;
}

PyObject*
BB_FromArray(PyObject* self, PyObject* args, PyObject* kwargs) {
    PyObject* array_like;
    static char* kwlist[] = {"array_like", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &array_like)) {
        PyErr_SetString(PyExc_ValueError, "Failed to parse the arguments");
        return NULL;
    }

    uint64 bb = bb_from_object(array_like);
    if (!bb && PyErr_Occurred()) {
        return NULL;  // Propagate error if conversion failed
    }

    return (PyObject*)(PyObject*)PyBitBoard_FromUnsignedLongLong(bb);
}

PyObject*
BB_FromSquares(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* sequence;
    static char* kwlist[] = {"squares", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &sequence)) {
        PyErr_SetString(PyExc_ValueError, "Failed to parse the arguments");
        return NULL;
    }

    if (!PySequence_Check(sequence)){
        PyErr_Format(
            PyExc_TypeError,
            "squares must be a python object sqeuence like (list, tuple, ...)."
            "got %s",
            Py_TYPE(sequence)->tp_name
        );
        return NULL;
    }

    PyObject* sqr_obj;
    uint64 bb = 0ULL;
    Py_ssize_t len = PySequence_Size(sequence);
    if (PyErr_Occurred())
        return NULL;

    Square sqr;

    for (Py_ssize_t i = 0; i < len; i++){
        sqr_obj = PySequence_GetItem(sequence, i);
        if (!sqr_obj){
            if (!PyErr_Occurred()){
                PyErr_SetString(
                    PyExc_ValueError,
                    "faild to get item from sequence object"
                );
            }
            return NULL;
        }

        sqr = pyobject_as_square(sqr_obj);
        if (sqr == NCH_NO_SQR){
            if (!PyErr_Occurred()){
                 PyErr_SetString(
                    PyExc_ValueError,
                    "NO_SQURE is invalid for this function"
                );
            }
            Py_DECREF(sqr_obj);
            return NULL;
        }

        bb |= NCH_SQR(sqr);

        Py_DECREF(sqr_obj);
    }

    return (PyObject*)PyBitBoard_FromUnsignedLongLong(bb);
}

PyObject*
BB_Between(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* src, *dst;
    static char* kwlist[] = {"src_square", "dst_square", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "KO", kwlist, &src, &dst)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    Square dst_sqr = pyobject_as_square(src);
    if (dst_sqr == NCH_NO_SQR) Py_RETURN_NONE;

    Square src_sqr = pyobject_as_square(dst);
    if (src_sqr == NCH_NO_SQR) Py_RETURN_NONE;

    return (PyObject*)PyBitBoard_FromUnsignedLongLong(bb_between(src_sqr, dst_sqr));
}

PyObject*
BB_RookAttacks(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* sqr;
    PyObject* occupancy;
    static char* kwlist[] = {"square", "occupancy", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", kwlist, &sqr, &occupancy)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    uint64 bb = bb_from_object(occupancy);
    if (!bb && PyErr_Occurred())
        return NULL;

    Square s = pyobject_as_square(sqr);
    if (s == NCH_NO_SQR)
        return NULL;

    return (PyObject*)PyBitBoard_FromUnsignedLongLong(bb_rook_attacks(s, bb));
}

PyObject*
BB_BishopAttacks(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* sqr;
    PyObject* occupancy;
    static char* kwlist[] = {"square", "occupancy", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", kwlist, &sqr, &occupancy)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    uint64 bb = bb_from_object(occupancy);
    if (!bb && PyErr_Occurred())
        return NULL;

    Square s = pyobject_as_square(sqr);
    if (s == NCH_NO_SQR)
        return NULL;

    return (PyObject*)PyBitBoard_FromUnsignedLongLong(bb_bishop_attacks(s, bb));
}

PyObject*
BB_QueenAttacks(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* sqr;
    PyObject* occupancy;
    static char* kwlist[] = {"square", "occupancy", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "OO", kwlist, &sqr, &occupancy)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    uint64 bb = bb_from_object(occupancy);
    if (!bb && PyErr_Occurred())
        return NULL;

    Square s = pyobject_as_square(sqr);
    if (s == NCH_NO_SQR)
        return NULL;

    return (PyObject*)PyBitBoard_FromUnsignedLongLong(bb_queen_attacks(s, bb));
}

PyObject*
BB_KingAttacks(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* sqr;
    static char* kwlist[] = {"square", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &sqr)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    Square s = pyobject_as_square(sqr);
    if (s == NCH_NO_SQR)
        return NULL;

    return (PyObject*)PyBitBoard_FromUnsignedLongLong(bb_king_attacks(s));
}

PyObject*
BB_KnightAttacks(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* sqr;
    static char* kwlist[] = {"square", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &sqr)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    Square s = pyobject_as_square(sqr);
    if (s == NCH_NO_SQR)
        return NULL;

    return (PyObject*)PyBitBoard_FromUnsignedLongLong(bb_knight_attacks(s));
}

PyObject*
BB_PawnAttacks(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* sqr;
    int white;
    static char* kwlist[] = {"square", "white", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Op", kwlist, &sqr, &white)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    Square s = pyobject_as_square(sqr);
    if (s == NCH_NO_SQR)
        return NULL;

    Side side = white ? NCH_White : NCH_Black;

    return (PyObject*)PyBitBoard_FromUnsignedLongLong(bb_pawn_attacks(side, s));
}

PyObject*
BB_RookMask(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* sqr;
    static char* kwlist[] = {"square", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &sqr)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    Square s = pyobject_as_square(sqr);
    if (s == NCH_NO_SQR)
        return NULL;

    return (PyObject*)PyBitBoard_FromUnsignedLongLong(bb_rook_mask(s));
}

PyObject*
BB_BishopMask(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* sqr;
    static char* kwlist[] = {"square", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &sqr)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    Square s = pyobject_as_square(sqr);
    if (s == NCH_NO_SQR)
        return NULL;

    return (PyObject*)PyBitBoard_FromUnsignedLongLong(bb_bishop_mask(s));
}

PyObject*
BB_RookRelevant(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* sqr;
    static char* kwlist[] = {"square", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &sqr)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    Square s = pyobject_as_square(sqr);
    if (s == NCH_NO_SQR)
        return NULL;

    return PyLong_FromLong(bb_rook_relevant(s));
}

PyObject*
BB_BishopRelevant(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* sqr;
    static char* kwlist[] = {"square", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &sqr)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    Square s = pyobject_as_square(sqr);
    if (s == NCH_NO_SQR)
        return NULL;

    return PyLong_FromLong(bb_bishop_relevant(s));
}

PyObject*
BB_RookMagic(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* sqr;
    static char* kwlist[] = {"square", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &sqr)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    Square s = pyobject_as_square(sqr);
    if (s == NCH_NO_SQR)
        return NULL;

    return (PyObject*)PyBitBoard_FromUnsignedLongLong(bb_rook_magic(s));
}

PyObject*
BB_BishopMagic(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* sqr;
    static char* kwlist[] = {"square", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &sqr)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    Square s = pyobject_as_square(sqr);
    if (s == NCH_NO_SQR)
        return NULL;

    return (PyObject*)PyBitBoard_FromUnsignedLongLong(bb_bishop_magic(s));
}

PyObject* BB_ToIndeices(PyObject* self, PyObject* args, PyObject* kwargs){
    uint64 bb;
    if (parse_bb(&bb, args) < 0)
        return NULL;

    PyObject* list = PyList_New(count_bits(bb));
    if (!list)
        return NULL;

    int idx;
    Py_ssize_t i = 0;
    LOOP_U64_T(bb){
        PyList_SetItem(list, i++, PyLong_FromLong(idx));
    }

    return list;
}