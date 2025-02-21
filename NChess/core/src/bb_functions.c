#include "bb_functions.h"
#include "nchess/bit_operations.h"
#include "array_conversion.h"
#include "common.h"
#include <numpy/arrayobject.h>

NCH_STATIC int
parse_bb(uint64* bb, PyObject* args){
    if (!PyArg_ParseTuple(args, "K", bb)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return -1;
    }
    return 0;
}

PyObject* 
BB_AsArray(PyObject* self, PyObject* args, PyObject* kwargs){
    int reversed = 0;
    int as_list = 0;
    uint64 bb;

    int nitems = NCH_SQUARE_NB;
    npy_intp dims[NPY_MAXDIMS];
    int ndim = parse_bb_conversion_function_args(&bb, nitems, dims, args, kwargs, &reversed, &as_list);

    if (ndim < 0)
        return NULL;

    if (!ndim){
        ndim = 1;
        dims[0] = nitems;
    }

    if (as_list){
        int data[NCH_SQUARE_NB];
        bb2array(bb, data, reversed);
        return create_list_array(data, dims, ndim);
    }
    else{
        int* data = (int*)malloc(nitems * sizeof(int));
        if (!data){
            PyErr_NoMemory();
            return NULL;
        }

        bb2array(bb, data, reversed);
        
        PyObject* array = create_numpy_array(data, dims, ndim, NPY_INT);
        if (!array){
            free(data);
            if (!PyErr_Occurred()){
                PyErr_SetString(PyExc_RuntimeError, "Failed to create array");
            }
            return NULL;
        }

        return array;
    }
}

PyObject*
BB_MoreThenOne(PyObject* self, PyObject* args){
    uint64 bb;
    if (parse_bb(&bb, args) < 0)
        return NULL;

    return PyBool_FromLong(more_then_one(bb));
}

PyObject*
BB_HasTwoBits(PyObject* self, PyObject* args){
    uint64 bb;
    if (parse_bb(&bb, args) < 0)
        return NULL;

    return PyBool_FromLong(has_two_bits(bb));
}

PyObject*
BB_GetTSB(PyObject* self, PyObject* args){
    uint64 bb;
    if (parse_bb(&bb, args) < 0)
        return NULL;

    return PyLong_FromLong(get_last_bit(bb));
}

PyObject*
BB_GetLSB(PyObject* self, PyObject* args){
    uint64 bb;
    if (parse_bb(&bb, args) < 0)
        return NULL;

    return PyLong_FromLong(get_ls1b(bb));
}

PyObject*
BB_CountBits(PyObject* self, PyObject* args){
    uint64 bb;
    if (parse_bb(&bb, args) < 0)
        return NULL;

    return PyLong_FromLong(count_bits(bb));
}


PyObject*
BB_IsFilled(PyObject* self, PyObject* args, PyObject* kwargs){
    uint64 bb;
    PyObject* sqr;
    static char* kwlist[] = {"bb", "square", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "KO", kwlist, &bb, &sqr)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    Square s = pyobject_as_square(sqr);
    if (s == NCH_NO_SQR)
        Py_RETURN_NONE;

    return PyBool_FromLong(bb & NCH_SQR(s));
}

PyObject*
BB_Between(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* src, *dst;
    static char* kwlist[] = {"src_square", "dst_square2", NULL};

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

    return PyLong_FromUnsignedLongLong(bb_between(src_sqr, dst_sqr));
}

NCH_STATIC_INLINE int
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
    return dim;
}

NCH_STATIC int
seq2bb(PyObject* seq, uint64* bb, Py_ssize_t* idx){
    Py_ssize_t len = PySequence_Size(seq);
    if (len < 0){
        PyErr_SetString(PyExc_ValueError, "falid to get the length of the sequence object");
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
            if (*idx < 64){
                *bb |= PyLong_AsLong(item) ? NCH_SQR(*idx) : 0;
            }
            else{
                PyErr_SetString(PyExc_ValueError, "bitboard sequence should have 64 nitems. got more");
                Py_DECREF(item);
                return -1;
            }
            (*idx)++;
        }
        else if (PySequence_Check(item)){
            res = seq2bb(item, bb, idx);
            if (res < 0){
                Py_DECREF(item);
                return -1;
            }
        }
        else{
            PyErr_Format(PyExc_ValueError,
            "bitboard sequence should contain int or sqeunece type objects (list, tuple, ...), got %s",
            Py_TYPE(item)->tp_name);
            
            Py_DECREF(item);
            return -1;
        }
        Py_DECREF(item);
    }

    return 0;
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
        return PyLong_AsUnsignedLongLong(obj);
    }

    import_array();
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
BB_FromArray(PyObject* self, PyObject* args, PyObject* kwargs){
    PyObject* array_like;
    static char* kwlist[] = {"array_like", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &array_like)){
        if (!PyErr_Occurred()){
            PyErr_SetString(PyExc_ValueError, "failed to parse the arguments");
        }
        return NULL;
    }

    uint64 bb = bb_from_object(array_like);
    if (!bb && PyErr_Occurred())
        return NULL;

    return PyLong_FromUnsignedLongLong(bb);
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

    return PyLong_FromUnsignedLongLong(bb_rook_attacks(s, bb));
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

    return PyLong_FromUnsignedLongLong(bb_bishop_attacks(s, bb));
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

    return PyLong_FromUnsignedLongLong(bb_queen_attacks(s, bb));
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

    return PyLong_FromUnsignedLongLong(bb_king_attacks(s));
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

    return PyLong_FromUnsignedLongLong(bb_knight_attacks(s));
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

    return PyLong_FromUnsignedLongLong(bb_pawn_attacks(side, s));
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

    return PyLong_FromUnsignedLongLong(bb_rook_mask(s));
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

    return PyLong_FromUnsignedLongLong(bb_bishop_mask(s));
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

    return PyLong_FromUnsignedLongLong(bb_rook_magic(s));
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

    return PyLong_FromUnsignedLongLong(bb_bishop_magic(s));
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