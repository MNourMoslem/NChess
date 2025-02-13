/*
    bit_operations.h

    this file contains bit operations functions and macros
    used in the project.
    it is too obvious that these functions are used to manipulate
    bits but it is kind of a habit to write this explenation in every
    file.

    future updates:
        - get_ls1b would be deleted and get_ts1b would be renamed something like
          get_last_bit.
*/


#ifndef NCHESS_SRC_BIT_OPERATIONS_H
#define NCHESS_SRC_BIT_OPERATIONS_H

#include "config.h"
#include "types.h"


NCH_STATIC_INLINE int
count_bits(uint64 x){
    #if NCH_GCC
        return __builtin_popcountll(x);
    #elif NCH_MSC
        return __popcnt64(x);
    #else
        uint64 count = 0;
        while(x){
            x &= x - 1;
            count++;
        }
        return count;
    #endif
};

NCH_STATIC_INLINE int
count_lbits(uint64 x){
    #if NCH_GCC
        return __builtin_clzll(x);
    #elif NCH_MSC
        unsigned long index;
        _BitScanReverse64(&index, x);
        return 63 - index;
    #else
        uint64 count = 0;
        if (x == 0) return 64;
        while (!(x & (1ULL << 63))) {
            x <<= 1;
            count++;
        }
        return count;
    #endif 
};

NCH_STATIC_INLINE int
count_tbits(uint64 x){
    #if NCH_GCC
        return __builtin_ctzll(x);
    #elif NCH_MSC
        unsigned long index;
        _BitScanForward64(&index, x);
        return index;
    #else
        uint64 count = 0;
        if (x == 0) return 64;
        while (!(x & 1)) {
            x >>= 1;
            count++;
        }
        return count;
    #endif
};

NCH_STATIC_INLINE uint64
get_ls1b(uint64 x) {
    return x & -x;
}

NCH_STATIC_INLINE uint64
get_ts1b(uint64 x) {
    return x & ~(x - 1);
}

NCH_STATIC_INLINE int
more_then_one(uint64 x){
    return (x & (x - 1)) != 0;
}

NCH_STATIC_INLINE int
more_then_two(uint64 x){
    x = x & (x - 1);
    x = x & (x - 1);
    return x != 0;
}

NCH_STATIC_INLINE int
has_two_bits(uint64 x){
    return !more_then_one(x & (x-1));
}

#endif // NCHESS_SRC_BIT_OPERATIONS_H