#ifndef NCHESS_SRC_BIT_OPERATIONS_H
#define NCHESS_SRC_BIT_OPERATIONS_H

#include "config.h"
#include "types.h"

#define NCH_CLZLL(x) __builtin_clzll(x)
#define NCH_CTZLL(x) __builtin_ctzll(x)
#define NCH_POPCOUNT(x) __builtin_popcount(x)
#define NCH_POPCOUNTLL(x) __builtin_popcountll(x)

#define NCH_HEADER NCH_STATIC NCH_INLINE

NCH_HEADER uint64
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

NCH_HEADER uint64
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

NCH_HEADER uint64
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

NCH_HEADER uint64
get_ls1b(uint64 x) {
    return x & -x;
}

NCH_HEADER uint64
get_ts1b(uint64 x) {
    return x & ~(x - 1);
}

#endif // NCHESS_SRC_BIT_OPERATIONS_H