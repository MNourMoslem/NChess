#ifndef NCHESS_SRC_INCLUDE_CONFIG_H
#define NCHESS_SRC_INCLUDE_CONFIG_H

// Detect Compiler
#if defined(__GNUC__) || defined(__clang__)  // GCC and Clang
    #define NCH_INLINE __inline__
    #define NCH_NOINLINE __attribute__((noinline))
    #define NCH_FINLINE __attribute__((always_inline)) __inline__
    #define NCH_TLS __thread
#elif defined(_MSC_VER)  // Microsoft Visual Studio Compiler
    #define NCH_INLINE __inline
    #define NCH_NOINLINE __declspec(noinline)
    #define NCH_FINLINE __forceinline
    #define NCH_TLS __declspec(thread)
#else  // Other compilers
    #define NCH_INLINE inline
    #define NCH_NOINLINE
    #define NCH_FINLINE inline
    #define NCH_TLS
#endif

#define NCH_STATIC static
#define NCH_STATIC_INLINE NCH_STATIC NCH_INLINE
#define NCH_STATIC_FINLINE NCH_STATIC NCH_FINLINE

#define NCH_NULL ((void*)0)

#if defined(__GNUC__)
    #define __COMP_NCH_UNUSED __attribute__ ((__unused__))
#elif defined(__ICC)
    #define __COMP_NCH_UNUSED __attribute__ ((__unused__))
#elif defined(__clang__)
    #define __COMP_NCH_UNUSED __attribute__ ((unused))
#else
    #define __COMP_NCH_UNUSED
#endif

#define NCH_UNUSED(x) _##x##__COMP_NPY_UNUSED

#endif /* NCH_CONFIG_H */